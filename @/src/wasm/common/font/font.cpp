#include "font.h"
#include "dirent.h"
#include <string>
#include <string.h>

const char *font_get_root()
{
    return FONT_ROOT_DIR;
}

std::string font_get_default_font()
{
    DIR *font_root=opendir(font_get_root());
    if(font_root==NULL)
    {
        return "";
    }
    dirent *ent=NULL;
    while((ent=readdir(font_root))!=NULL)
    {
        if(ent->d_type==DT_REG)
        {
            std::string filename=ent->d_name;
            if(filename.substr(filename.length()-4)==std::string(".ttf") || filename.substr(filename.length()-4)==std::string(".TTF"))
            {
                closedir(font_root);
                return filename;
            }
        }
    }
    closedir(font_root);
    return "";

}

