/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <Modules/CoreCG/DesktopRenderer.hxx>

namespace CG::WM
{
	struct CG_WINDOW_STRUCT;

	enum
	{
		cWndFlagNoShow			   = 0x02,
		cWndFlagButton			   = 0x04,
		cWndFlagWindow			   = 0x06,
		cWndFlagButtonSelect	   = 0x08,
		cWndFlagNoCloseButton	   = 0x010,
		cWndFlagCloseButton		   = 0x012,
		cWndFlagSelectBox		   = 0x014,
		cWndFlagInputBox		   = 0x016,
		cWndFlagCheckBoxFirstState = 0x018,
		cWndFlagCheckBoxTwoState   = 0x018,
		cWndFlagCheckBoxThreeState = 0x018,
	};

	struct CG_WINDOW_STRUCT final
	{
		static constexpr auto cChildElementCount = 4096;

		Char					 w_window_name[255]{0};
		Char					 w_class_name[255]{0};
		Int32					 w_type{0};
		Int32					 w_sub_type{0};
		Int32					 w_x{0};
		Int32					 w_y{0};
		Int32					 w_w{0};
		Int32					 w_h{0};
		Size					 w_child_count{0};
		struct CG_WINDOW_STRUCT* w_child_list[cChildElementCount]{0};
		struct CG_WINDOW_STRUCT* w_parent_wnd{nullptr};
		UInt32*					 w_canvas{nullptr};
		Bool					 w_needs_repaint{false};
	};

	typedef struct CG_WINDOW_STRUCT CG_WINDOW_STRUCT;

	inline struct CG_WINDOW_STRUCT* CGCreateWindow(Int32 kind, const Char* window_name, const Char* class_name, Int32 x, Int32 y, Int32 width, Int32 height, CG_WINDOW_STRUCT* parent = nullptr)
	{
		CG_WINDOW_STRUCT* wnd = new CG_WINDOW_STRUCT();

		if (!wnd)
		{
			ErrLocal() = kErrorHeapOutOfMemory;
			return nullptr;
		}

		rt_copy_memory((VoidPtr)window_name, wnd->w_window_name, rt_string_len(window_name));
		rt_copy_memory((VoidPtr)class_name, wnd->w_class_name, rt_string_len(class_name));

		if (parent)
		{
			wnd->w_parent_wnd = parent;

			++wnd->w_parent_wnd->w_child_count;
			wnd->w_parent_wnd->w_child_list[wnd->w_parent_wnd->w_child_count - 1] = wnd;
		}

		wnd->w_sub_type = 0;
		wnd->w_type		= kind;
		wnd->w_x		= x;
		wnd->w_y		= y;

		wnd->w_w = width;
		wnd->w_h = height;

		wnd->w_canvas = new UInt32[width * height];
		rt_set_memory(wnd->w_canvas, CGColor(0xF5, 0xF5, 0xF5), width * height);

		return wnd;
	}

	/// \brief Destroys a window and it's contents.
	inline Bool CGDestroyWindow(struct CG_WINDOW_STRUCT* wnd)
	{
		if (wnd)
		{
			if (!mm_is_valid_heap(wnd))
			{
				wnd = nullptr;
				return true;
			}

			wnd->w_needs_repaint = No;

			for (SizeT index = 0UL; index < wnd->w_child_count; ++index)
			{
				CGDestroyWindow(wnd->w_child_list[index]);
			}

			delete wnd;
			wnd = nullptr;

			return true;
		}

		return false;
	}

	inline Kernel::Void CGDrawStringToWnd(CG_WINDOW_STRUCT* wnd, const Kernel::Char* text, Kernel::Int32 y_dst, Kernel::Int32 x_dst, Kernel::Int32 color)
	{
		y_dst += wnd->w_y + FLATCONTROLS_HEIGHT;
		x_dst += wnd->w_x;

		if (y_dst > (wnd->w_h + wnd->w_y))
			return;

		for (Kernel::SizeT i = 0; text[i] != 0; ++i)
		{
			if (x_dst > (wnd->w_w + wnd->w_x))
				break;

			CGRenderStringFromBitMap(&cFontBitmap[text[i]][0], FONT_SIZE_X, FONT_SIZE_Y, y_dst, x_dst, color);
			x_dst += FONT_SIZE_Y;
		}
	}

	inline SizeT CGDrawWindowList(CG_WINDOW_STRUCT** wnd, SizeT wnd_cnt)
	{
		if (wnd_cnt == 0 ||
			!wnd)
			return 0;

		SizeT cnt = 0;

		for (SizeT index = 0; index < wnd_cnt; ++index)
		{
			if (!wnd[index] ||
				(wnd[index]->w_type == cWndFlagNoShow) ||
				!wnd[index]->w_needs_repaint)
				continue;

			CGInit();

			wnd[index]->w_needs_repaint = false;

			if (CGAccessibilty::The().Width() < wnd[index]->w_x)
			{
				if ((wnd[index]->w_x - CGAccessibilty::The().Width()) > 1)
				{
					wnd[index]->w_x -= wnd[index]->w_x - CGAccessibilty::The().Width();
				}
				else
				{
					wnd[index]->w_x = 0;
				}
			}

			if (CGAccessibilty::The().Height() < wnd[index]->w_y)
			{
				if ((wnd[index]->w_y - CGAccessibilty::The().Height()) > 1)
				{
					wnd[index]->w_y -= wnd[index]->w_y - CGAccessibilty::The().Width();
				}
				else
				{
					wnd[index]->w_y = 0;
				}
			}

			++cnt;

			// Draw fake controls, just for the looks of it (WINDOW ONLY)
			if (wnd[index]->w_type == cWndFlagWindow)
			{
				CGDrawBitMapInRegion(wnd[index]->w_canvas, wnd[index]->w_h, wnd[index]->w_w, wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(CGColor(0xFF, 0xFF, 0xFF), wnd[index]->w_w, FLATCONTROLS_HEIGHT, wnd[index]->w_y, wnd[index]->w_x);

				if (wnd[index]->w_sub_type != cWndFlagNoCloseButton)
				{
					if (wnd[index]->w_sub_type == cWndFlagCloseButton)
					{
						CGDrawBitMapInRegion(FlatControlsClose, FLATCONTROLS_CLOSE_HEIGHT, FLATCONTROLS_CLOSE_WIDTH, wnd[index]->w_y, wnd[index]->w_x + wnd[index]->w_w - FLATCONTROLS_WIDTH);
					}
					else
					{
						CGDrawBitMapInRegion(FlatControls, FLATCONTROLS_HEIGHT, FLATCONTROLS_WIDTH, wnd[index]->w_y, wnd[index]->w_x + wnd[index]->w_w - FLATCONTROLS_WIDTH);
					}
				}

				CGDrawString(wnd[index]->w_window_name, wnd[index]->w_y + 8, wnd[index]->w_x + 8, CGColor(0x00, 0x00, 0x00));
			}
			/// @note buttons in this library are dynamic, it's because we want to avoid as much as computations as possible.
			/// (Such as getting the middle coordinates of a button, to center the text.)
			else if (wnd[index]->w_type == cWndFlagButtonSelect)
			{
				auto x_center = wnd[index]->w_x + 6;
				auto y_center = wnd[index]->w_y + 7;

				CGDrawInRegion(CGColor(0xD3, 0x74, 0x00), wnd[index]->w_w + 1, wnd[index]->w_h + 1, wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(CGColor(0xFF, 0xFF, 0xFF), wnd[index]->w_w - 1, wnd[index]->w_h - 1, wnd[index]->w_y + 1, wnd[index]->w_x + 1);
				CGDrawStringToWnd(wnd[index], wnd[index]->w_window_name, y_center, x_center, CGColor(0x00, 0x00, 0x00));
			}
			else if (wnd[index]->w_type == cWndFlagButton)
			{
				auto x_center = wnd[index]->w_x + 6;
				auto y_center = wnd[index]->w_y + 7;

				CGDrawInRegion(CGColor(0xDC, 0xDC, 0xDC), wnd[index]->w_w + 1, wnd[index]->w_h + 1, wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(CGColor(0xFF, 0xFF, 0xFF), wnd[index]->w_w - 1, wnd[index]->w_h - 1, wnd[index]->w_y + 1, wnd[index]->w_x + 1);
				CGDrawString(wnd[index]->w_window_name, y_center, x_center, CGColor(0x00, 0x00, 0x00));
			}

			CGFini();

			// draw child windows and controls.
			// doesn't have to be a window, enabling then windows in windows.

			for (SizeT child = 0; child < wnd[index]->w_child_count; ++child)
			{
				wnd[index]->w_child_list[child]->w_x += wnd[index]->w_x;
				wnd[index]->w_child_list[child]->w_y += wnd[index]->w_y + FLATCONTROLS_HEIGHT;

				if ((wnd[index]->w_child_list[child]->w_w + wnd[index]->w_child_list[child]->w_x) > (wnd[index]->w_x + wnd[index]->w_w) ||
					(wnd[index]->w_child_list[child]->w_h + wnd[index]->w_child_list[child]->w_y) > (wnd[index]->w_y + wnd[index]->w_h))
					continue;

				CGDrawWindowList(&wnd[index]->w_child_list[child], 1);
			}

			CGFini();
		}

		return cnt;
	}
} // namespace CG::WM