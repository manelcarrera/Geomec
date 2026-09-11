'$IFNDEF TREECONTROL
'$DEFINE TREECONTROL


' File name: TreeControl.inc
' Project:  Test
FUNCTION TreeItems(sControl As STRING, sItem As STRING) As ARRAY OF STRING

	DIM nStart as LONG				'Start position
	DIM index as STRING				'Index of item as string
	DIM ret as ARRAY OF STRING	'Array of returned strings
	DIM nSize As LONG
	Dim x as INTEGER
	Dim bWasExpanded as LONG

	bWasExpanded = true
	WTreeItemClk(sControl,sItem,VK_LBUTTON,60) ' allow for lengthy operations to return
	If Not WTreeItemExpanded(sControl,"") Then
		WTreeItemExpand(sControl,"")
		bWasExpanded = false
	endif
	nStart = WTreeItemIndex(sControl,"")
	nSize = WTreeItemCount(sControl,"")

	if nSize > 0 then
		REDIM ret(1 TO nSize) As STRING

		For x = 1 to nSize
		   index = _ord(x + nStart)
		   ret(x) = TreeItemText(sControl,index)
		Next x 

		if not bWasExpanded then
			WtreeItemCollapse(sControl,"")
		endif
	endif

	TreeItems = ret

END FUNCTION

' This function selects an item in the tree
FUNCTION SelectItem(sControl as STRING, sItem as STRING) as INTEGER

	WTreeItemClk(sControl,sItem)
	SelectItem = WTreeItemSelected(sControl,sItem)

END FUNCTION

SUB DumpTree(sControl as STRING, startItem as STRING, level = 0 as INTEGER )

	Dim forms as ARRAY of STRING
	Dim e as LONG
	forms = TreeItems(sControl,startItem)
	if not isempty(forms) then
		for e = lbound(forms) to ubound(forms)
			print string(level *4," ") + forms(e)
			DumpTree (sControl , startItem + "\" + forms(e), level + 1)
		next
	endif

END SUB

'$ENDIF