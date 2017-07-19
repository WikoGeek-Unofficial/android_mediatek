#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/mtd/mtd.h>
#include "ipanic.h"

struct ipanic_mtd_data {
	struct mtd_info	*mtd;
	u32 blk_nr;
	int bufsize;
	u64 buf;
	u32 blk_offset[1024];
};

static struct ipanic_mtd_data *ipanic_mtd_ctx;

static int ipanic_mtd_block_read_single(struct ipanic_mtd_data *ctx, loff_t offset, void *buf)
{
	int rc, len;
	int index = offset >> ctx->mtd->writesize_shift;

	if ((index < 0) || (index >= ctx->blk_nr)) {
		return -EINVAL;
	}

	rc = ctx->mtd->_read(ctx->mtd, ctx->blk_offset[index], ctx->mtd->writesize, &len, buf);
#if 0
	if (rc == -EBADMSG) {
		IPANIC_W("Check sum error (ignore)\n");
		rc = 0;
	}
#endif
	if (rc == -EUCLEAN) {
		IPANIC_W("ECC Check sum error corrected %lld\n", offset);
		rc = 0;
	}
	if ((rc == 0) && (len != ctx->mtd->writesize)) {
		IPANIC_E("aee-ipanic: read size mismatch %d\n", len);
		return -EINVAL;
	}
	return rc;
}

static int ipanic_mtd_block_read(struct ipanic_mtd_data *ctx, off_t file_offset,
				int file_length, void *buf, void *tmp) {
#if 0
	IPANIC_I("read: file_offset:%d file_length:%lu\n", file_offset, file_length);
#endif
	while (file_length > 0) {
		unsigned int page_no;
		off_t page_offset;
		int rc;
		size_t count = file_length;

		/* We only support reading a maximum of a flash page */
		if (count > ctx->mtd->writesize)
			count = ctx->mtd->writesize;
		page_no = file_offset / ctx->mtd->writesize;
		page_offset = file_offset % ctx->mtd->writesize;

		rc = ipanic_mtd_block_read_single(ctx, page_no * ctx->mtd->writesize, tmp);
		if (rc < 0) {
			IPANIC_E("mtd read error page_no(%d) error(%d)\n", page_no, rc);
			goto error_return;
		}
		if (page_offset)
			count -= page_offset;
		memcpy(buf, tmp + page_offset, count);

		file_length -= count;
		buf += count;
		file_offset += count;
	}
	return 0;
error_return:
	return -1;
}

static int ipanic_mtd_block_write(struct ipanic_mtd_data *ctx, loff_t to, int bounce_len, void *buf)
{
	int rc;
	size_t wlen;
	int panic = in_interrupt() | in_atomic();
	int index = to >> ctx->mtd->writesize_shift;

	if ((index < 0) || (index >= ctx->blk_nr)) {
		return -EINVAL;
	}

	if (bounce_len > ctx->mtd->writesize) {
		IPANIC_E("%s: len too large[%x]\n", __func__, bounce_len);
		return -EINVAL;
	}
	if (panic && !ctx->mtd->_panic_write) {
		IPANIC_E("%s: No panic_write available\n", __func__);
		return 0;
	} else if (!panic && !ctx->mtd->_write) {
		IPANIC_E("%s: No write available\n", __func__);
		return 0;
	}

	if (bounce_len < ctx->mtd->writesize)
		memset(buf + bounce_len, 0, ctx->mtd->writesize - bounce_len);

	if (panic)
		rc = ctx->mtd->_panic_write(ctx->mtd, ctx->blk_offset[index], ctx->mtd->writesize, &wlen, buf);
	else
		rc = ctx->mtd->_write(ctx->mtd, ctx->blk_offset[index], ctx->mtd->writesize, &wlen, buf);

	if (rc) {
		IPANIC_E("%s: Error writing data to flash (%d)\n", __func__, rc);
		return rc;
	}

	return wlen;
}

static int ipanic_mtd_block_scan(struct ipanic_mtd_data *ctx) 
{
	int index = 0, offset;
	
	/* calcuate total number of non-bad blocks on NAND device,  */
	/* and record it's offset */
	for (offset = 0; offset < ctx->mtd->size; offset += ctx->mtd->writesize) {
		
		if (!ctx->mtd->_block_isbad(ctx->mtd, offset)) {

			/*index can't surpass array size*/
			if(index >= (sizeof(ctx->blk_offset)/ sizeof(u32))) {
				break;
			}

			ctx->blk_offset[index++] = offset;
		}
	}
	ctx->blk_nr = index;

#if 0
	IPANIC_I("blocks: ");
	for (index = 0; index < ctx->blk_nr; index++) {
		IPANIC_I("%x,", ctx->blk_offset[index]);
	}
	IPANIC_I("\n");
#endif
	return 1;
}

static void ipanic_mtd_block_erase_callback(struct erase_info *done)
{
	wait_queue_head_t *wait_q = (wait_queue_head_t *) done->priv;
	wake_up(wait_q);
}

static void ipanic_mtd_block_erase(void)
{
	struct ipanic_mtd_data *ctx = ipanic_mtd_ctx;
	struct erase_info erase;
	DECLARE_WAITQUEUE(wait, current);
	wait_queue_head_t wait_q;
	int rc, i;

	init_waitqueue_head(&wait_q);
	erase.mtd = ctx->mtd;
	erase.callback = ipanic_mtd_block_erase_callback;
	erase.len = ctx->mtd->erasesize;
	erase.priv = (u_long)&wait_q;
	for (i = 0; i < ctx->mtd->size; i += ctx->mtd->erasesize) {
		erase.addr = i;
		set_current_state(TASK_INTERRUPTIBLE);
		add_wait_queue(&wait_q, &wait);

		rc = ctx->mtd->_block_isbad(ctx->mtd, erase.addr);
		if (rc < 0) {
			IPANIC_E("Bad block check failed (%d)\n", rc);
			goto out;
		}
		if (rc) {
			IPANIC_W("Skipping erase of bad block @%llx\n", erase.addr);
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&wait_q, &wait);
			continue;
		}

		rc = ctx->mtd->_erase(ctx->mtd, &erase);
		if (rc) {
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&wait_q, &wait);
			IPANIC_E("Erase of 0x%llx, 0x%llx failed\n",
			       (unsigned long long) erase.addr,
			       (unsigned long long) erase.len);
			if (rc == -EIO) {
				if (ctx->mtd->_block_markbad(ctx->mtd, erase.addr)) {
					IPANIC_E("aee-ipanic: Err marking blk bad\n");
					goto out;
				}
				IPANIC_I("Marked a bad block @%llx\n", erase.addr);
				continue;
			}
			goto out;
		}
		schedule();
		remove_wait_queue(&wait_q, &wait);
	}
	IPANIC_D("%s partition erased\n", AEE_IPANIC_PLABEL);
out:
	return;
}

static void ipanic_mtd_notify_add(struct mtd_info *mtd)
{
	static struct ipanic_mtd_data mtd_drv_ctx;
	struct ipanic_mtd_data *ctx = &mtd_drv_ctx;

	if (strcmp(mtd->name, AEE_IPANIC_PLABEL))
		return;

	ctx->mtd = mtd;

	if (!ipanic_mtd_block_scan(ctx))
		goto out_err;
	IPANIC_I("Bound to '%s', write size(%d), write size shift(%d)\n",
		 mtd->name, mtd->writesize, mtd->writesize_shift);
	ctx->bufsize = ALIGN(PAGE_SIZE, mtd->writesize);
	ctx->buf = (u64)(int)kmalloc(ctx->bufsize, GFP_KERNEL);
	ipanic_mtd_ctx = ctx;
	return;

out_err:
	ctx->mtd = NULL;
}

static void ipanic_mtd_notify_remove(struct mtd_info *mtd)
{
	struct ipanic_mtd_data *ctx = ipanic_mtd_ctx;
	if (mtd == ctx->mtd) {
		ctx->mtd = NULL;
		IPANIC_I("aee-ipanic: Unbound from %s\n", mtd->name);
	}
}

static struct mtd_notifier ipanic_mtd_notifier = {
	.add	= ipanic_mtd_notify_add,
	.remove	= ipanic_mtd_notify_remove,
};

void ipanic_msdc_init(void) {
	register_mtd_user(&ipanic_mtd_notifier);
}
EXPORT_SYMBOL(ipanic_msdc_init);

int ipanic_msdc_info(struct ipanic_header *iheader) {
	if (ipanic_mtd_ctx == NULL || ipanic_mtd_ctx->mtd == NULL) {
		IPANIC_E("%s: mtd is not ready.", __func__);
		return -1;
	}
	iheader->blksize = ipanic_mtd_ctx->mtd->writesize;
	iheader->partsize = ipanic_mtd_ctx->blk_nr * iheader->blksize;
	iheader->buf = ipanic_mtd_ctx->buf;
	iheader->bufsize = ipanic_mtd_ctx->bufsize;
	if (iheader->buf == 0) {
		IPANIC_E("kmalloc fail[%x]\n", iheader->bufsize);
		return -1;
	}
	return 0;
}
EXPORT_SYMBOL(ipanic_msdc_info);

char *ipanic_read_size(int off, int len) {
	int size;
	char *buff = NULL;
	struct ipanic_header *ipanic_hdr = ipanic_header();

	if (ipanic_mtd_ctx == NULL || ipanic_mtd_ctx->mtd == NULL) {
		IPANIC_E("%s: mtd is not ready.", __func__);
		return NULL;
	}
	if (len == 0)
		return NULL;
	size = ALIGN(len, ipanic_hdr->blksize);
	buff = kzalloc(size, GFP_KERNEL);
	if (buff == NULL) {
		IPANIC_E("%s: cannot allocate buffer(len:%d)\n", __func__, len);
		return NULL;
	}
	if (ipanic_mtd_block_read(ipanic_mtd_ctx, off, size, buff, (void*)(u32)ipanic_hdr->buf) != 0) {
		IPANIC_E("%s: read failed(offset:%d,size:%d)\n", __func__, off, size);
		kfree(buff);
		return NULL;
	}
	return buff;
}
EXPORT_SYMBOL(ipanic_read_size);

int ipanic_write_size(void *buf, int off, int len) {
	int wlen = 0;
	int start = off;
	struct ipanic_header *iheader = ipanic_header();

	if (ipanic_mtd_ctx == NULL || ipanic_mtd_ctx->mtd == NULL) {
		IPANIC_E("%s: mtd is not ready.", __func__);
		return -1;
	}	
	if (len & (iheader->blksize - 1))
		return -2;
	while (len > 0) {
		wlen = len > ipanic_mtd_ctx->mtd->writesize ? ipanic_mtd_ctx->mtd->writesize : len;
		wlen = ipanic_mtd_block_write(ipanic_mtd_ctx, off, wlen, buf);
		if (wlen < 0) {
			IPANIC_E("%s: failed(%d)", __func__, wlen);
			return -1;
		}
		off += wlen;
		buf += wlen;
		len -= wlen;
	}
	return (off - start);
}
EXPORT_SYMBOL(ipanic_write_size);

void ipanic_erase(void) {
	ipanic_mtd_block_erase();
}
EXPORT_SYMBOL(ipanic_erase);
