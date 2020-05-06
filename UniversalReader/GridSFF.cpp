// class CGridSFF implementation file

#include "stdafx.h"
#include "GridSFF.h"

// ###################################################################
// CGridSFF GridCtrl
// ###################################################################
CGridSFF::CGridSFF()
{

}

CGridSFF::~CGridSFF()
{
}


void CGridSFF::Init()
{
	// Define Grid table common parameters 
	Config();

	// Set
	this->SetVirtualMode(m_bVirtualMode);
	this->SetEditable(m_bEditable);

	this->SetAutoSizeStyle();

	// Fill table with cells
	TRY
	{
		this->SetRowCount(m_nRows);
		this->SetColumnCount(m_nCols);
		this->SetFixedRowCount(m_nFixRows);
		this->SetFixedColumnCount(m_nFixCols);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return;
	}
	END_CATCH

		// > Set table appropriate size
		// header
		this->SetColumnWidth(0, 30);

	// cells
	for (int col = 1; col < this->GetColumnCount(); col++)
	{
		this->SetColumnWidth(col, 25);
	}


	// fill rows/cols with text
	for (int row = 0; row < this->GetRowCount(); row++)
	{
		for (int col = 0; col < this->GetColumnCount(); col++)
		{
			CString str;

			GV_ITEM Item;

			Item.mask = GVIF_TEXT;
			Item.row = row;
			Item.col = col;

			if (row < m_nFixRows)						// Title Fixed Row
			{
				if (col > 0)
				{
					str.Format(_T("%02X"), col - 1);
				}
			}
			else
			{
				if (col < m_nFixCols)					// Title Fixed Col
				{
					if (row > 0)
					{
						str.Format(_T("%02X"), row - 1);
						str.AppendChar('h');
					}
					else
					{
						str = "";
					}
				}
			}

			Item.strText = str;

			/*
			if (rand() % 10 == 1)
			{
			COLORREF clr = RGB(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);
			Item.crBkClr = clr;             // or - m_Grid.SetItemBkColour(row, col, clr);
			Item.crFgClr = RGB(255, 0, 0);    // or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));
			Item.mask |= (GVIF_BKCLR | GVIF_FGCLR);
			}

			if (col < this->GetFixedColumnCount())
			{
			//Item.iImage = rand() % m_ImageList.GetImageCount();
			Item.mask |= (GVIF_IMAGE);
			}
			*/
			this->SetItem(&Item);

		}
	}
	/*
	// Having weird Autosize problems with CE 2.11 - possibly just an emulation problem
	#if !defined(_WIN32_WCE) || (_WIN32_WCE < 211)
	m_Grid.AutoSize();
	#endif
	*/
}

void CGridSFF::Config()
{
	m_OldSize = CSize(-1, -1);

	//{{AFX_DATA_INIT(CGridCtrlDemoDlg)
	m_nFixCols = 1;
	m_nFixRows = 1;
	m_nCols = 16 + 1;							// +1 is for Fixed Cell
	m_nRows = 16 + 1;							// +1 is for Fixed Cell

	m_bEditable = TRUE;
	m_bHorzLines = FALSE;
	m_bVertLines = FALSE;
	m_bListMode = TRUE;
	m_bHeaderSort = FALSE;
	m_bSingleSelMode = TRUE;
	m_bSingleColSelMode = TRUE;
	m_bSelectable = FALSE;
	m_bAllowColumnResize = FALSE;
	m_bAllowRowResize = FALSE;
	m_bItalics = FALSE;
	m_btitleTips = FALSE;
	m_bTrackFocus = FALSE;
	m_bFrameFocus = FALSE;
	m_bVirtualMode = FALSE;
	m_bCallback = TRUE;
	m_bVertical = TRUE;
	m_bExpandUseFixed = FALSE;
	m_bRejectEditAttempts = TRUE;
	m_bRejectEditChanges = FALSE;
	m_bRow2Col2Hidden = FALSE;
}


// NOTE:
// FORMAT:
// # Orange:	/Vendor Name/	:		20-35	/254, 232, 185
// # Blue:		/Module Type/   :		40-55	/174, 238, 238
// # Red:		/Serial Number/ :		68-83	/255, 193, 194
// # Magenta:	/Data Code/		:		84-91	/238, 174, 235
// # Green:		/Checksum/		:		63, 95	/147, 243, 146
// #			/Length/		:		14-15
// # 
void CGridSFF::SetTableColor()
{
	// > Define Colours
	COLORREF clr_Red_head = RGB(255, 193, 194);
	COLORREF clr_Red_body = RGB(255, 223, 224);

	COLORREF clr_Orange_head = RGB(254, 232, 185);
	COLORREF clr_Orange_body = RGB(254, 252, 225);

	COLORREF clr_Blue_head = RGB(174, 238, 238);
	COLORREF clr_Blue_body = RGB(204, 255, 255);

	COLORREF clr_Magenta_head = RGB(238, 174, 235);
	COLORREF clr_Magenta_body = RGB(255, 234, 255);

	COLORREF clr_Green_head = RGB(167, 243, 166);
	COLORREF clr_Green_body = RGB(207, 250, 206);

	WORD uiCount;
	WORD uiStart;
	WORD byte_number;

	COLORREF Color_cur;

	BYTE uGridRow;
	BYTE uGridCol;

	WORD uGridColLast = 15 + 1;						// +1 from Header Col
	WORD uGridRowLast = 15 + 1;						// +1 from Header Row

	// > Set Cell Resource
	BYTE color_number = 0;

	while (color_number < 4)
	{
		switch (color_number)
		{
		case 0:	// define Range Orange const

			uiStart = 20;
			uiCount = 16;

			Color_cur = clr_Orange_body;

			break;

		case 1:	// define Range Blue const

			uiStart = 40;
			uiCount = 16;

			Color_cur = clr_Blue_body;

			break;

		case 2:	// define Range Red const

			uiStart = 68;
			uiCount = 16;

			Color_cur = clr_Red_body;

			break;

		case 3:	// define Range Magenta const

			uiStart = 84;
			uiCount = 8;

			Color_cur = clr_Magenta_body;

			break;

		default:
			break;
		}//switch (color_number)

		// > Current Color OP
		byte_number = uiCount;

		uGridRow = uiStart / 16;
		uGridCol = uiStart - uGridRow * 16;

		// fill in
		BYTE act = 1;
		while (act)
		{
			// SafeCheck: check Row end 
			if (uGridRow > uGridRowLast)
			{
				// exit
				// err
				act = 0;
			}

			// check Col end
			if (uGridCol > uGridColLast)
			{
				uGridCol = 1;

				uGridRow++;
			}

			// set Color
			this->SetItemBkColour(uGridRow, uGridCol, Color_cur);

			uGridCol++;

			// check End condition
			if (byte_number > 0)
			{
				byte_number--;
			}
			else
			{
				// [CYCLE COMPLETE]
				act = 0;
			}
		}//while (act) 

		// update End Condition Value
		color_number++;

	}//while (color_number < 4)
}

// clear selected cell-range
void CGridSFF::ClearCells(BYTE startAddr, WORD count)
{
	// define table start cell
	unsigned char stRow = startAddr / 0x10;				// start Row
	unsigned char stCol = startAddr - stRow * 0x10;		// start Col

														// fill in table
	unsigned char uRow = stRow + 1;						// +1 is for Fixed Cell
	unsigned char uCol = stCol + 1;						// +1 is for Fixed Cell

	for (WORD k = 0; k < count; k++)
	{
		// Clear Cell Text
		this->SetItemText(uRow, uCol, L"");

		// prepare next cell coord	
		uCol++;

		if (uCol > 0x0F + 1)							// +1 is for Fixed Cell
		{
			uCol = 0 + 1;								// +1 is for Fixed Cell
			uRow++;										// can't exceed 0x0F val cause of #SafeCheck
		}
	}

	this->Invalidate();

}


void CGridSFF::ClearTable()
{
	this->ClearCells(0, 256);

}

// Use: Update Grid[0xFF x 0xFF] Interface 
// FROM: startAddr TO: startAddr + count 
// with Memory Values[0xFF x 0xFF] 
// FROM: startAddr TO: startAddr + count
void CGridSFF::GridSFF_Write_UpdateRange(BYTE * v_ByteData, BYTE startAddr, WORD count)
{
	// Safe check
	if (startAddr + count > 256)
	{
		// out of range
		return;
	}
	else
	{
		// define table start cell
		unsigned char stRow = startAddr / 0x10;				// start Row
		unsigned char stCol = startAddr - stRow * 0x10;		// start Col

															// fill in table
		unsigned char uRow = stRow + 1;						// +1 is for Fixed Cell
		unsigned char uCol = stCol + 1;						// +1 is for Fixed Cell

		unsigned int k = 0;

		bool act = 1;
		do
		{
			// forming Cell Text
			CString cellText;
			cellText.Format(_T("%02X"), v_ByteData[startAddr + k]);
			this->SetItemText(uRow, uCol, cellText);

			// prepare next cell coord	/NOTE: useless at last cycle
			uCol++;

			if (uCol > 0x0F + 1)							// +1 is for Fixed Cell
			{
				uCol = 0 + 1;								// +1 is for Fixed Cell
				uRow++;										// can't exceed 0x0F val cause of #SafeCheck
			}

			k++;

		} while (k < count);

		this->Invalidate();
	} //SafeCheck
}

void CGridSFF::GridSFF_Write(BYTE * v_ByteData, BYTE startAddr, WORD count)
{
	// Safe check
	if (startAddr + count > 256)
	{
		// out of range
		return;
	}
	else
	{
		// define table start cell
		unsigned char stRow = startAddr / 0x10;				// start Row
		unsigned char stCol = startAddr - stRow * 0x10;		// start Col

															// fill in table
		unsigned char uRow = stRow + 1;						// +1 is for Fixed Cell
		unsigned char uCol = stCol + 1;						// +1 is for Fixed Cell

		unsigned int k = 0;

		bool act = 1;
		do
		{
			// forming Cell Text
			CString cellText;
			cellText.Format(_T("%02X"), v_ByteData[k]);
			this->SetItemText(uRow, uCol, cellText);

			// prepare next cell coord	/NOTE: useless at last cycle
			uCol++;

			if (uCol > 0x0F + 1)							// +1 is for Fixed Cell
			{
				uCol = 0 + 1;								// +1 is for Fixed Cell
				uRow++;										// can't exceed 0x0F val cause of #SafeCheck
			}

			k++;

		} while (k < count);

		this->Invalidate();
	} //SafeCheck
}

void CGridSFF::GridSFF_Write_Byte(BYTE * v_ByteData, BYTE startAddr, WORD count)
{
	// Safe check
	if (startAddr + count > 256)
	{
		// out of range
		return;
	}
	else
	{
		// define table start cell
		unsigned char stRow = startAddr / 0x10;				// start Row
		unsigned char stCol = startAddr - stRow * 0x10;		// start Col

															// fill in table
		unsigned char uRow = stRow + 1;						// +1 is for Fixed Cell
		unsigned char uCol = stCol;					

		unsigned int k = 0;

		bool act = 1;
		do
		{
			// forming Cell Text
			CString cellText;
			cellText.Format(_T("%c"), v_ByteData[startAddr + k]);
			this->SetItemText(uRow, uCol, cellText);

			// prepare next cell coord	/NOTE: useless at last cycle
			uCol++;

			if (uCol > 0x0F)							
			{
				uCol = 0;									
				uRow++;										// can't exceed 0x0F val cause of #SafeCheck
			}

			k++;

		} while (k < count);

		this->Invalidate();
	} //SafeCheck
}

void CGridSFF::GridHex16Cols_Write(BYTE* v_ByteData, BYTE startAddr, WORD count)
{
	// define table start cell
	WORD stRow = startAddr / 0x10;				// start Row
	WORD stCol = startAddr - stRow * 0x10;		// start Col

														// fill in table
	WORD uRow = stRow + 1;						// +1 is for Fixed Cell
	WORD uCol = stCol + 1;						// +1 is for Fixed Cell

	WORD k = 0;

	bool act = 1;
	do
	{
		// forming Cell Text
		CString cellText;
		cellText.Format(_T("%02X"), v_ByteData[startAddr + k]);
		this->SetItemText(uRow, uCol, cellText);

		// prepare next cell coord	/NOTE: useless at last cycle
		uCol++;

		if (uCol > 0x0F)
		{
			uCol = 0;
			uRow++;										// can't exceed 0x0F val cause of #SafeCheck
		}

		k++;

	} while (k < count);

	this->Invalidate();
}

// Use: Update Memory Values
// FROM: startAddr TO: startAddr + count
// with Grid Values
// FROM: startAddr TO: startAddr + count
void CGridSFF::GridSFF_Read_UpdateRange(BYTE * v_ByteData, BYTE startAddr, WORD count)
{
	// Safe check
	if (startAddr + count > 256)
	{
		// out of range
		return;
	}
	else
	{
		// define table start cell
		unsigned char stRow = startAddr / 0x10;				// start Row
		unsigned char stCol = startAddr - stRow * 0x10;		// start Col

															// get from Table
		unsigned char uRow = stRow + 1;						// +1 is for Fixed Cell
		unsigned char uCol = stCol + 1;						// +1 is for Fixed Cell

		unsigned int k = 0;

		bool act = 1;
		do
		{
			// extract Cell Text
			CString cellText = this->GetItemText(uRow, uCol);
			v_ByteData[startAddr + k] = (BYTE)_tcstoul(cellText, NULL, 16);

			// prepare next cell coord	/NOTE: useless at last cycle
			uCol++;

			if (uCol > 0x0F + 1)							// +1 is for Fixed Cell
			{
				uCol = 0 + 1;								// +1 is for Fixed Cell
				uRow++;										// can't exceed 0x0F val cause of #SafeCheck
			}

			k++;

		} while (k < count);

	}
}


// Use: simple Read Grid OP
// FROM: startAddr TO: startAddr + count
// in Memory
// FROM: 0 TO: count
void CGridSFF::GridSFF_Read(BYTE * v_ByteData, BYTE startAddr, WORD count)
{
	// Safe check
	if (startAddr + count > 256)
	{
		// out of range
		return;
	}
	else
	{
		// define table start cell
		unsigned char stRow = startAddr / 0x10;				// start Row
		unsigned char stCol = startAddr - stRow * 0x10;		// start Col

															// get from Table
		unsigned char uRow = stRow + 1;						// +1 is for Fixed Cell
		unsigned char uCol = stCol + 1;						// +1 is for Fixed Cell

		unsigned int k = 0;

		bool act = 1;
		do
		{
			// extract Cell Text
			CString cellText = this->GetItemText(uRow, uCol);
			v_ByteData[k] = (BYTE)_tcstoul(cellText, NULL, 16);

			// prepare next cell coord	/NOTE: useless at last cycle
			uCol++;

			if (uCol > 0x0F + 1)							// +1 is for Fixed Cell
			{
				uCol = 0 + 1;								// +1 is for Fixed Cell
				uRow++;										// can't exceed 0x0F val cause of #SafeCheck
			}

			k++;

		} while (k < count);

	}
}

int CGridSFF::GridSFF_CheckNull(BYTE startAddr, WORD count)
{
	// Safe check
	if (startAddr + count > 256)
	{
		// out of range
		return 1;
	}
	else
	{
		int bHasNullValue = 0;

		// define table start cell
		unsigned char stRow = startAddr / 0x10;				// start Row
		unsigned char stCol = startAddr - stRow * 0x10;		// start Col

															// get from Table
		unsigned char uRow = stRow + 1;						// +1 is for Fixed Cell
		unsigned char uCol = stCol + 1;						// +1 is for Fixed Cell

		unsigned int k = 0;
		do
		{
			// extract Cell Text
			CString cellText = this->GetItemText(uRow, uCol);
			
			// check NULL
			if ((cellText == "") || (cellText == " "))
			{
				// [NULL]

				bHasNullValue = 1;

				break;
			}

			// prepare next cell coord	/NOTE: useless at last cycle
			uCol++;

			if (uCol > 0x0F + 1)							// +1 is for Fixed Cell
			{
				uCol = 0 + 1;								// +1 is for Fixed Cell
				uRow++;										// can't exceed 0x0F val cause of #SafeCheck
			}

			k++;

		} while (k < count);

		return bHasNullValue;
	}	
}

int CGridSFF::CheckValidHex(int iRow, int iCol)
{
	// error symbol at first
	bool act = 1;

	CString cellText = this->GetItemText(iRow, iCol);

	// check valid Length
	if (cellText.GetLength() == 2)
	{
		// check valid Symbols
		CString hexString = cellText.SpanIncluding(_T("0123456789abcdefABCDEF"));

		if (hexString.Compare(cellText) == 0)
		{
			act = 0;

			// Change to upper case
			this->SetItemText(iRow, iCol, cellText.MakeUpper());
		}
	}

	return act;
}

int CGridSFF::CheckValidASCII(int iRow, int iCol)
{
	bool act = 0;

	CString cellText = this->GetItemText(iRow, iCol);

	// check valid Length
	if (cellText.GetLength() != 1)
	{
		// error symbol
		act = 1;
	}

	return act;
}


int CGridSFF::LoadFromFile(BYTE * v_ByteData, char * filename, BYTE ucLineCount, BYTE ucOffset)
{
	// CONST
	const unsigned char uLineCount = ucLineCount;
	const unsigned char uByteCount = 16;
	const unsigned char uLetterCount = 2;

	// err status
	unsigned char errLine;

	// position
	unsigned char l;				// active Line
	unsigned char k;				// active Byte
	unsigned char i;				// active Letter (Hex)

									// str Hex buffer
	unsigned char chex_buf[2];

	FILE *fp;


	// [#] FILE OP CHECK

	// > Open File try
	errno_t err;

	err = fopen_s(&fp, filename, "r");

	if (err != 0)
	{	// "can't open file" 
		errLine = -1;
		return OP_BADNAME;
	}

	// > Define parameters for probress bar
	// count number of Intel Hex file lines
	unsigned int fileLineNum = 0;
	char *e_str;
	char IHline[256];
	while (TRUE)
	{
		e_str = fgets(IHline, sizeof(IHline), fp);
		fileLineNum++;

		// exit condition
		if (e_str == NULL)
		{
			// check line read condition
			if (feof(fp) != 0)
			{	// end of file

				break;
			}
			else
			{	// file corrupted
				errLine = -2;
				//		return MAXQBL_OP_FAILED;
				break;
			}
		}
	}

	// reopen FILE
	fclose(fp);

	err = fopen_s(&fp, filename, "r");

	if (err != 0)
	{	// "can't open file" 
		errLine = -1;

		return OP_BADNAME;
	}

	// [STATUS OUTPUT]
	//Trace(_T("- lines: %03d \n"), fileLineNum);


	// > Read Data

	// init cycle Read var
	l = 0;
	k = 0;
	i = 0;
	chex_buf[0] = '0';
	chex_buf[1] = '0';


	// Read OP

	// NOTE:
	// FORMAT of file: 
	// H0 H1 H2 H3 H4 H5 H6 H7 H8 H9 HA HB HC HD HE HF // (Line 0x00)
	// ...
	// H0 H1 H2 H3 H4 H5 H6 H7 H8 H9 HA HB HC HD HE HF // (Line 0x0F)
	// where HN - Hex String symbols

	bool ACT = 1;						// reading file op is active
	while (ACT)
	{	// proceed symbol routine

		// > Get symbol
		int c = fgetc(fp);

		// > Select type
		switch (c)
		{
		case EOF:						// [#] cancel on End of File
			ACT = FALSE;
			break;

		case '\n':						// [#] prepare to New Line

			l++;						// line number

			if (l > uLineCount - 1)		// check End of Reading OP
			{
				ACT = FALSE;
			}
			else
			{
				// reset byte number in Line
				k = 0;
			}
			break;

		case '\t':						// [#] prepare to New Byte

			k++;						// Byte number

			if (k > uByteCount - 1)		// check End of Line
			{
				// NOP
				// !probably set error here
			}

			break;


		default:						// [#] all Common text letter symbols
										// [#] Parce Intel Hex line
										// read letter
			chex_buf[i] = c;

			if (i == 0)
			{
				// FIRST letter
				i++;
			}
			else
			{
				// SECOND letter

				// > check valid Symbols
				// make hex string
				CString fileText;
				fileText.AppendChar(chex_buf[0]);
				fileText.AppendChar(chex_buf[1]);

				// valid symbol checking
				CString hexTextValid = fileText.SpanIncluding(_T("0123456789abcdefABCDEF"));

				if (hexTextValid.Compare(fileText) == 0)
				{
					// convert to Byte
					// place to uValues[] Table

					// check Offset
					if (k >= ucOffset)
					{
						// [OFFSET COMPLETED]

						v_ByteData[l * uByteCount + k - ucOffset] = (BYTE)_tcstoul(hexTextValid, NULL, 16);
					}					

					// reset letter number
					i = 0;
				}
				else
				{
					// set err number equal to Line number
					errLine = l;

					return OP_FAILED;
				} // if compare
			}

			break;

		} // switch c
	} // while ACT


	// > Close Intel Hex File
	fclose(fp);


	return OP_SUCCESS;
}


int CGridSFF::LoadFromFile_FullTable(BYTE * v_ByteData, char * filename)
{
	int iResult = LoadFromFile(v_ByteData, filename, 16, 0);

	GridSFF_Write(v_ByteData, 0, 256);

	return iResult;
}


int CGridSFF::LoadFromFile_StandardTable(BYTE * v_ByteData, char * filename)
{
	int iResult = LoadFromFile(v_ByteData, filename, 8, 0);

	GridSFF_Write(v_ByteData, 0, 128);

	return iResult;
}


int CGridSFF::LoadFromFile_AdditionTable_Up(BYTE * v_ByteData, char * filename)
{
	int iResult = LoadFromFile(v_ByteData, filename, 8, 128);

	GridSFF_Write(v_ByteData, 0, 128);

	return iResult;
}


int CGridSFF::LoadFromFile_AdditionTable_Down(BYTE * v_ByteData, char * filename)
{
	int iResult = LoadFromFile(v_ByteData, filename, 8, 128);

	GridSFF_Write(v_ByteData, 128, 128);

	return iResult;
}


int CGridSFF::SaveToFile(BYTE * v_ByteData, char * filename, BYTE ucLineCount, BYTE usOffset)
{
	// CONST
	// # define COUNT of Byte in Line
	const unsigned char LINE_BYTE_COUNT = 16;

	const unsigned char uLineCount = ucLineCount;
	const unsigned char uByteCount = 16;
	const unsigned char uLetterCount = 2;

	// err status
	unsigned char errLine;

	// position
	unsigned char l;				// active Line
	unsigned char k;				// active Byte
	unsigned char i;				// active Letter (Hex)

									// str Hex buffer
	unsigned char chex_buf[2];

	FILE *fp;


	// [#] FILE OP CHECK
	BYTE err = 0;

	err = fopen_s(&fp, filename, "w");

	if (err != 0)
	{	// "can't open file" 
		errLine = -1;
		return OP_FAILED;
	}


	// > Write Data

	// NOTE:
	// FORMAT of file: 
	// H0 H1 H2 H3 H4 H5 H6 H7 H8 H9 HA HB HC HD HE HF // (Line 0x00)
	// ...
	// H0 H1 H2 H3 H4 H5 H6 H7 H8 H9 HA HB HC HD HE HF // (Line 0x0F)
	// where HN - Hex String symbols

	// init cycle Write var
	l = 0;
	k = 0;

	//
	unsigned char cbyte_line[LINE_BYTE_COUNT];

	bool ACT = 1;							// writing file op is active
	while (ACT)
	{
		// fill the Line
		cbyte_line[k] = v_ByteData[l * uByteCount + k];

		if (k > uByteCount - 2)				// check End of Line
		{
			// > forming Line

			// make string of hex
			CString fileHexSymbol;
			CString fileHexLine;

			for (unsigned char ik = 0; ik < uByteCount; ik++)
			{
				fileHexSymbol.Format(_T("%02X"), cbyte_line[ik]);

				fileHexLine.Append(fileHexSymbol);
				fileHexLine.AppendChar('\t');
			}

			// make null_string Line_of_char to write in file
			// NOTE:
			// FORMAT: 'H' 'H' ' ', ... , 'H' 'H' \n \0
			char chex_line[uByteCount * 3 + 1];

			for (unsigned char ik = 0; ik < uByteCount * 3 - 1; ik++)
			{
				chex_line[ik] = fileHexLine[ik];
			}

			// add line ender
			chex_line[uByteCount * 3 - 1] = '\n';
			chex_line[uByteCount * 3] = '\0';

			// > write string to file
			// check offset
			if (l >= usOffset)
			{
				// [OFFSET COMPLETED]

				fputs(chex_line, fp);
			}
			

			if (l > uLineCount - 2)			// check End of Reading OP
			{
				ACT = FALSE;
			}
			else
			{
				// prepare new Line
				l++;

				// reset Byte number in Line
				k = 0;
			}
		}
		else
		{
			k++;
		}
	} // 


	  // > Close Intel Hex File
	fclose(fp);


	return OP_SUCCESS;
}


int CGridSFF::SaveToFile_FullTable(char * filename)
{
	BYTE v_ByteData[256];

	GridSFF_Read(v_ByteData, 0, 256);

	int iResult = SaveToFile(v_ByteData, filename, 16, 0);

	return iResult;
}


int CGridSFF::SaveToFile_StandardTable(char * filename)
{
	BYTE v_ByteData[128];

	GridSFF_Read(v_ByteData, 0, 128);

	int iResult = SaveToFile(v_ByteData, filename, 8, 0);

	return iResult;
}


int CGridSFF::SaveToFile_AdditionTable(char * filename)
{
	BYTE v_ByteData[128];

	GridSFF_Read(v_ByteData, 128, 128);

	int iResult = SaveToFile(v_ByteData, filename, 8 , 0);

	return iResult;
}