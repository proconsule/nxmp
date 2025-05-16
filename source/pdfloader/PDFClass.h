#include <cstdlib>
#include <string>

#include <mupdf/fitz.h>

class CPDFClass{
public:
    CPDFClass(unsigned char* _data,size_t _size);
    int Render_PDF_Page(int _page,float _zoom,unsigned char ** _image_data,int *_w,int *_h);

    int getPageCount(){
        return pagecount;
    }
    int getCurrentPage(){
        return pagenum;
    }
    
    bool isOK(){
        return document_ok;
    }
    
        
private:
    bool document_ok = false;
    unsigned char *filebuffer;
    size_t filesize = 0;
    int pagenum = 0;
    int pagecount = -1;
    
    fz_context *ctx;
    fz_document *doc;
    
};