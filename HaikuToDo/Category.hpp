#ifndef CATEGORY_HPP
#define CATEGORY_HPP

#include <InterfaceKit.h>
#include <ControlLook.h>
#include <IconUtils.h>
#include <vector>

class Category : public BListItem{
	public:
		Category(const char* name, const char* filepath) : name(name)
		{
			icon=new BBitmap(BRect(0,0,15,15),B_RGBA32);
			if(strcmp(filepath,"MIME_DATABASE")!=0)
			{
				BFile iconPath(filepath,B_READ_ONLY);
				if(iconPath.InitCheck() != B_OK)
					return;
				off_t file_size=0;
				iconPath.GetSize(&file_size);
				std::cout << "Get size: " << file_size << std::endl;
				//void* buffer=malloc(file_size*sizeof(char)+10);
				char buffer[file_size+1024];
				iconPath.Read(buffer,file_size);
				if(BIconUtils::GetVectorIcon((uint8*)buffer,sizeof(buffer),icon)!=B_OK)
				{
					std::cerr << "Error while loading vector icon " << std::endl;
				}
				//free(buffer);
				std::cout << "Readed vector icon" << std::endl;
			}else{
				BNode iconPath("/boot/system/data/mime_db/text/plain");
				BIconUtils::GetVectorIcon(&iconPath,"META:ICON",icon);
			}
		}
		
		~Category()
		{
			
		}
		
		const char*
		GetName()
		{
			return name.String();
		}
	private:
		void DrawItem(BView* owner, BRect frame, bool complete)
		{
			float iconSize=icon->Bounds().Width();
			//float iconSize=frame.bottom;
			if(IsSelected() || complete)
			{
				rgb_color color;
				if(IsSelected())
				{
					color=ui_color(B_LIST_SELECTED_BACKGROUND_COLOR);
				}else{
					color=owner->ViewColor();
				}
				owner->SetHighColor(color);
				owner->FillRect(frame);
			}
			owner->MovePenTo(frame.left+iconSize+8,frame.bottom-2);
			owner->SetHighColor(ui_color(B_LIST_ITEM_TEXT_COLOR));
			owner->DrawString(name);
			

			
			BRect iconFrame(frame.left-be_control_look->DefaultLabelSpacing(),
					frame.top,
					frame.left + iconSize -1 + be_control_look->DefaultLabelSpacing(),
					frame.top + iconSize -1
			);
			owner->SetDrawingMode(B_OP_OVER);
			owner->DrawBitmap(icon,iconFrame);
			owner->SetDrawingMode(B_OP_COPY);
			
		}
		BString name;
		BBitmap* icon;
	
};

#endif
