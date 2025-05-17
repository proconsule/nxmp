#include "PDFClass.h"


CPDFClass::CPDFClass(unsigned char* _data,size_t _size){
        filebuffer = _data;
        filesize = _size;
        ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fprintf(stderr, "cannot create mupdf context\n");
		return;
	}

	fz_try(ctx)
		fz_register_document_handlers(ctx);
	fz_catch(ctx)
	{
		fprintf(stderr, "cannot register document handlers\n");
		fz_drop_context(ctx);
		return;
	}

	/* Open the document. */
	fz_buffer *pdfbuffer =  fz_new_buffer_from_data(ctx,_data,_size);
        
        fz_stream *stream = fz_open_buffer(ctx, pdfbuffer);
	fz_try(ctx)
		doc = fz_open_document_with_stream(ctx, "pdf", stream);
        fz_always(ctx)
		fz_drop_stream(ctx, stream);
        fz_catch(ctx)
	{
		fprintf(stderr, "cannot open document\n");
		fz_drop_context(ctx);
		return;
	}
        fz_try(ctx)
		pagecount = fz_count_pages(ctx, doc);
        fz_catch(ctx)
	{
		fprintf(stderr, "cannot count number of pages\n");
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return;
	}
        
    
        document_ok = true;
}

int CPDFClass::Render_PDF_Page(int _page,float _zoom,unsigned char ** _image_data,int *_w,int *_h){
    fz_context *ctx;
    fz_document *doc;
    fz_pixmap *pix;
    float rotate;
    fz_matrix ctm;
    int x, y;
    
    
    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fprintf(stderr, "cannot create mupdf context\n");
		return EXIT_FAILURE;
	}

	/* Register the default file types to handle. */
	fz_try(ctx)
		fz_register_document_handlers(ctx);
	fz_catch(ctx)
	{
		fprintf(stderr, "cannot register document handlers\n");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}
        
        fz_buffer *pdfbuffer =  fz_new_buffer_from_data(ctx,filebuffer,filesize);
        
        fz_stream *stream = fz_open_buffer(ctx, pdfbuffer);
	fz_try(ctx)
		doc = fz_open_document_with_stream(ctx, "pdf", stream);
        fz_always(ctx)
		fz_drop_stream(ctx, stream);
        fz_catch(ctx)
		{
			fprintf(stderr, "cannot open document\n");
			fz_drop_context(ctx);
			return EXIT_FAILURE;
		}
        
        
        ctm = fz_scale(_zoom / 100, _zoom / 100);
	ctm = fz_pre_rotate(ctm, rotate);
    
        
        fz_try(ctx)
		pix = fz_new_pixmap_from_page_number(ctx, doc, _page, ctm, fz_device_rgb(ctx), 0);
		fz_catch(ctx)
		{
			fprintf(stderr, "cannot render page\n");
			fz_drop_document(ctx, doc);
			fz_drop_context(ctx);
			return EXIT_FAILURE;
		}
        
        *_w = pix->w;
        *_h = pix->h;
        unsigned int rgblen = pix->w*pix->h*3;
        unsigned int rgbalen = pix->w*pix->h*4;
        unsigned char * rga_data = (unsigned char *)malloc(pix->w*pix->h*4);
        
        int j=0;
        for(int i=0;i<rgblen;i=i+3){
            rga_data[j] = pix->samples[i];
            rga_data[j+1] = pix->samples[i+1];
            rga_data[j+2] = pix->samples[i+2];
            rga_data[j+3] = 255;
            
            
            j=j+4;
        }
        
        *_image_data = rga_data;
        
		
		pagenum = _page;
		
		fz_drop_pixmap(ctx, pix);
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		
        return 0;
    
}