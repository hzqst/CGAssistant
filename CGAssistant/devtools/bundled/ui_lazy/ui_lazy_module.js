UI.ChartViewport=class extends UI.VBox{constructor(){super(true);this.viewportElement=this.contentElement.createChild('div','fill');this.viewportElement.addEventListener('mousewheel',this._onMouseWheel.bind(this),false);this.viewportElement.addEventListener('keydown',this._handleZoomPanKeys.bind(this),false);UI.installInertialDragHandle(this.viewportElement,this._startDragging.bind(this),this._dragging.bind(this),this._endDragging.bind(this),'-webkit-grabbing',null);UI.installDragHandle(this.viewportElement,this._startRangeSelection.bind(this),this._rangeSelectionDragging.bind(this),this._endRangeSelection.bind(this),'text',null);this._vScrollElement=this.contentElement.createChild('div','flame-chart-v-scroll');this._vScrollContent=this._vScrollElement.createChild('div');this._vScrollElement.addEventListener('scroll',this._onScroll.bind(this),false);this._selectionOverlay=this.contentElement.createChild('div','flame-chart-selection-overlay hidden');this._selectedTimeSpanLabel=this._selectionOverlay.createChild('div','time-span');this.reset();}
isDragging(){return this._isDragging;}
elementsToRestoreScrollPositionsFor(){return[this._vScrollElement];}
_updateScrollBar(){var showScroll=this._totalHeight>this._offsetHeight;if(this._vScrollElement.classList.contains('hidden')!==showScroll)
return;this._vScrollElement.classList.toggle('hidden',!showScroll);this._updateContentElementSize();}
onResize(){this._updateScrollBar();this._updateContentElementSize();this.scheduleUpdate();}
reset(){this._vScrollElement.scrollTop=0;this._scrollTop=0;this._rangeSelectionStart=0;this._rangeSelectionEnd=0;this._isDragging=false;this._dragStartPointX=0;this._dragStartPointY=0;this._dragStartScrollTop=0;this._timeWindowLeft=0;this._timeWindowRight=0;this._offsetWidth=0;this._offsetHeight=0;this._totalHeight=0;this._pendingAnimationTimeLeft=0;this._pendingAnimationTimeRight=0;}
_updateContentElementSize(){var offsetWidth=this._vScrollElement.offsetLeft;if(!offsetWidth)
offsetWidth=this.contentElement.offsetWidth;this._offsetWidth=offsetWidth;this._offsetHeight=this.contentElement.offsetHeight;}
setContentHeight(totalHeight){this._totalHeight=totalHeight;this._vScrollContent.style.height=totalHeight+'px';this._updateScrollBar();if(this._scrollTop+this._offsetHeight<=totalHeight)
return;this._scrollTop=Math.max(0,totalHeight-this._offsetHeight);this._vScrollElement.scrollTop=this._scrollTop;}
setScrollOffset(offset,height){height=height||0;if(this._vScrollElement.scrollTop>offset)
this._vScrollElement.scrollTop=offset;else if(this._vScrollElement.scrollTop<offset-this._offsetHeight+height)
this._vScrollElement.scrollTop=offset-this._offsetHeight+height;}
scrollOffset(){return this._vScrollElement.scrollTop;}
_onMouseWheel(e){if(!this._enabled())
return;var panVertically=e.shiftKey&&(e.wheelDeltaY||Math.abs(e.wheelDeltaX)===120);var panHorizontally=Math.abs(e.wheelDeltaX)>Math.abs(e.wheelDeltaY)&&!e.shiftKey;if(panVertically){this._vScrollElement.scrollTop-=(e.wheelDeltaY||e.wheelDeltaX)/120*this._offsetHeight/8;}else if(panHorizontally){var shift=-e.wheelDeltaX*this._pixelToTime;this._muteAnimation=true;this._handlePanGesture(shift);this._muteAnimation=false;}else{const mouseWheelZoomSpeed=1/120;this._handleZoomGesture(Math.pow(1.2,-(e.wheelDeltaY||e.wheelDeltaX)*mouseWheelZoomSpeed)-1);}
e.consume(true);}
_startDragging(x,y,event){if(event.shiftKey)
return false;if(this._windowRight===Infinity)
return false;this._isDragging=true;this._initMaxDragOffset(event);this._dragStartPointX=x;this._dragStartPointY=y;this._dragStartScrollTop=this._vScrollElement.scrollTop;this.viewportElement.style.cursor='';this.hideHighlight();return true;}
_dragging(x,y){var pixelShift=this._dragStartPointX-x;this._dragStartPointX=x;this._muteAnimation=true;this._handlePanGesture(pixelShift*this._pixelToTime);this._muteAnimation=false;var pixelScroll=this._dragStartPointY-y;this._vScrollElement.scrollTop=this._dragStartScrollTop+pixelScroll;this._updateMaxDragOffset(x,y);}
_endDragging(){this._isDragging=false;this._updateHighlight();}
_initMaxDragOffset(event){this._maxDragOffsetSquared=0;this._dragStartX=event.pageX;this._dragStartY=event.pageY;}
_updateMaxDragOffset(x,y){var dx=x-this._dragStartX;var dy=y-this._dragStartY;var dragOffsetSquared=dx*dx+dy*dy;this._maxDragOffsetSquared=Math.max(this._maxDragOffsetSquared,dragOffsetSquared);}
maxDragOffset(){return Math.sqrt(this._maxDragOffsetSquared);}
_startRangeSelection(event){if(!event.shiftKey)
return false;this._isDragging=true;this._initMaxDragOffset(event);this._selectionOffsetShiftX=event.offsetX-event.pageX;this._selectionOffsetShiftY=event.offsetY-event.pageY;this._selectionStartX=event.offsetX;var style=this._selectionOverlay.style;style.left=this._selectionStartX+'px';style.width='1px';this._selectedTimeSpanLabel.textContent='';this._selectionOverlay.classList.remove('hidden');this.hideHighlight();return true;}
_endRangeSelection(){this._isDragging=false;this._updateHighlight();}
hideRangeSelection(){this._selectionOverlay.classList.add('hidden');}
_rangeSelectionDragging(event){this._updateMaxDragOffset(event.pageX,event.pageY);var x=Number.constrain(event.pageX+this._selectionOffsetShiftX,0,this._offsetWidth);var start=this._cursorTime(this._selectionStartX);var end=this._cursorTime(x);this._rangeSelectionStart=Math.min(start,end);this._rangeSelectionEnd=Math.max(start,end);this._updateRangeSelectionOverlay();this._flameChartDelegate.updateRangeSelection(this._rangeSelectionStart,this._rangeSelectionEnd);}
_updateRangeSelectionOverlay(){var margin=100;var left=Number.constrain(this._timeToPosition(this._rangeSelectionStart),-margin,this._offsetWidth+margin);var right=Number.constrain(this._timeToPosition(this._rangeSelectionEnd),-margin,this._offsetWidth+margin);var style=this._selectionOverlay.style;style.left=left+'px';style.width=(right-left)+'px';var timeSpan=this._rangeSelectionEnd-this._rangeSelectionStart;this._selectedTimeSpanLabel.textContent=Number.preciseMillisToString(timeSpan,2);}
_onScroll(){this._scrollTop=this._vScrollElement.scrollTop;this.scheduleUpdate();}
_handleZoomPanKeys(e){if(!UI.KeyboardShortcut.hasNoModifiers(e))
return;var zoomMultiplier=e.shiftKey?0.8:0.3;var panMultiplier=e.shiftKey?320:80;if(e.code==='KeyA'){this._handlePanGesture(-panMultiplier*this._pixelToTime);e.consume(true);}else if(e.code==='KeyD'){this._handlePanGesture(panMultiplier*this._pixelToTime);e.consume(true);}else if(e.code==='KeyW'){this._handleZoomGesture(-zoomMultiplier);e.consume(true);}else if(e.code==='KeyS'){this._handleZoomGesture(zoomMultiplier);e.consume(true);}}
_handleZoomGesture(zoom){this._cancelAnimation();var bounds=this._windowForGesture();var cursorTime=this._cursorTime(this._lastMouseOffsetX);bounds.left+=(bounds.left-cursorTime)*zoom;bounds.right+=(bounds.right-cursorTime)*zoom;this._requestWindowTimes(bounds);}
_handlePanGesture(shift){this._cancelAnimation();var bounds=this._windowForGesture();shift=Number.constrain(shift,this._minimumBoundary-bounds.left,this._totalTime+this._minimumBoundary-bounds.right);bounds.left+=shift;bounds.right+=shift;this._requestWindowTimes(bounds);}
_windowForGesture(){var windowLeft=this._timeWindowLeft?this._timeWindowLeft:this._dataProvider.minimumBoundary();var windowRight=this._timeWindowRight!==Infinity?this._timeWindowRight:this._dataProvider.minimumBoundary()+this._dataProvider.totalTime();return{left:windowLeft,right:windowRight};}
_requestWindowTimes(bounds){bounds.left=Number.constrain(bounds.left,this._minimumBoundary,this._totalTime+this._minimumBoundary);bounds.right=Number.constrain(bounds.right,this._minimumBoundary,this._totalTime+this._minimumBoundary);if(bounds.right-bounds.left<UI.FlameChart.MinimalTimeWindowMs)
return;this._flameChartDelegate.requestWindowTimes(bounds.left,bounds.right);}
_animateWindowTimes(startTime,endTime){this._timeWindowLeft=startTime;this._timeWindowRight=endTime;this._updateHighlight();this.update();}
_animationCompleted(){delete this._cancelWindowTimesAnimation;this._updateHighlight();}
_cancelAnimation(){if(!this._cancelWindowTimesAnimation)
return;this._timeWindowLeft=this._pendingAnimationTimeLeft;this._timeWindowRight=this._pendingAnimationTimeRight;this._cancelWindowTimesAnimation();delete this._cancelWindowTimesAnimation;}
scheduleUpdate(){if(this._updateTimerId||this._cancelWindowTimesAnimation)
return;this._updateTimerId=this.element.window().requestAnimationFrame(()=>{this._updateTimerId=0;this.update();});}
update(){}
setWindowTimes(startTime,endTime){this.hideRangeSelection();if(this._muteAnimation||this._timeWindowLeft===0||this._timeWindowRight===Infinity||(startTime===0&&endTime===Infinity)||(startTime===Infinity&&endTime===Infinity)){this._timeWindowLeft=startTime;this._timeWindowRight=endTime;this.scheduleUpdate();return;}
this._cancelAnimation();this._cancelWindowTimesAnimation=UI.animateFunction(this.element.window(),this._animateWindowTimes.bind(this),[{from:this._timeWindowLeft,to:startTime},{from:this._timeWindowRight,to:endTime}],5,this._animationCompleted.bind(this));this._pendingAnimationTimeLeft=startTime;this._pendingAnimationTimeRight=endTime;}};;UI.DataGrid=class extends Common.Object{constructor(columnsArray,editCallback,deleteCallback,refreshCallback){super();this.element=createElementWithClass('div','data-grid');UI.appendStyle(this.element,'ui_lazy/dataGrid.css');this.element.tabIndex=0;this.element.addEventListener('keydown',this._keyDown.bind(this),false);this.element.addEventListener('contextmenu',this._contextMenu.bind(this),true);this._editCallback=editCallback;this._deleteCallback=deleteCallback;this._refreshCallback=refreshCallback;var headerContainer=this.element.createChild('div','header-container');this._headerTable=headerContainer.createChild('table','header');this._headerTableHeaders={};this._scrollContainer=this.element.createChild('div','data-container');this._dataTable=this._scrollContainer.createChild('table','data');if(editCallback)
this._dataTable.addEventListener('dblclick',this._ondblclick.bind(this),false);this._dataTable.addEventListener('mousedown',this._mouseDownInDataTable.bind(this));this._dataTable.addEventListener('click',this._clickInDataTable.bind(this),true);this._inline=false;this._columnsArray=[];this._columns={};this._visibleColumnsArray=columnsArray;columnsArray.forEach(column=>this._innerAddColumn(column));this._cellClass=null;this._headerTableColumnGroup=this._headerTable.createChild('colgroup');this._headerTableBody=this._headerTable.createChild('tbody');this._headerRow=this._headerTableBody.createChild('tr');this._dataTableColumnGroup=this._dataTable.createChild('colgroup');this.dataTableBody=this._dataTable.createChild('tbody');this._topFillerRow=this.dataTableBody.createChild('tr','data-grid-filler-row revealed');this._bottomFillerRow=this.dataTableBody.createChild('tr','data-grid-filler-row revealed');this.setVerticalPadding(0,0);this._refreshHeader();this._editing=false;this.selectedNode=null;this.expandNodesWhenArrowing=false;this.setRootNode(new UI.DataGridNode());this.indentWidth=15;this._resizers=[];this._columnWidthsInitialized=false;this._cornerWidth=UI.DataGrid.CornerWidth;this._resizeMethod=UI.DataGrid.ResizeMethod.Nearest;this._headerContextMenuCallback=null;this._rowContextMenuCallback=null;}
headerTableBody(){return this._headerTableBody;}
_innerAddColumn(column,position){var columnId=column.id;if(columnId in this._columns)
this._innerRemoveColumn(columnId);if(position===undefined)
position=this._columnsArray.length;this._columnsArray.splice(position,0,column);this._columns[columnId]=column;if(column.disclosure)
this.disclosureColumnId=columnId;var cell=createElement('th');cell.className=columnId+'-column';cell[UI.DataGrid._columnIdSymbol]=columnId;this._headerTableHeaders[columnId]=cell;var div=createElement('div');if(column.titleDOMFragment)
div.appendChild(column.titleDOMFragment);else
div.textContent=column.title;cell.appendChild(div);if(column.sort){cell.classList.add(column.sort);this._sortColumnCell=cell;}
if(column.sortable){cell.addEventListener('click',this._clickInHeaderCell.bind(this),false);cell.classList.add('sortable');cell.createChild('div','sort-order-icon-container').createChild('div','sort-order-icon');}}
addColumn(column,position){this._innerAddColumn(column,position);}
_innerRemoveColumn(columnId){var column=this._columns[columnId];if(!column)
return;delete this._columns[columnId];var index=this._columnsArray.findIndex(columnConfig=>columnConfig.id===columnId);this._columnsArray.splice(index,1);var cell=this._headerTableHeaders[columnId];if(cell.parentElement)
cell.parentElement.removeChild(cell);delete this._headerTableHeaders[columnId];}
removeColumn(columnId){this._innerRemoveColumn(columnId);}
setCellClass(cellClass){this._cellClass=cellClass;}
_refreshHeader(){this._headerTableColumnGroup.removeChildren();this._dataTableColumnGroup.removeChildren();this._headerRow.removeChildren();this._topFillerRow.removeChildren();this._bottomFillerRow.removeChildren();for(var i=0;i<this._visibleColumnsArray.length;++i){var column=this._visibleColumnsArray[i];var columnId=column.id;var headerColumn=this._headerTableColumnGroup.createChild('col');var dataColumn=this._dataTableColumnGroup.createChild('col');if(column.width){headerColumn.style.width=column.width;dataColumn.style.width=column.width;}
this._headerRow.appendChild(this._headerTableHeaders[columnId]);this._topFillerRow.createChild('td','top-filler-td');this._bottomFillerRow.createChild('td','bottom-filler-td')[UI.DataGrid._columnIdSymbol]=columnId;}
this._headerRow.createChild('th','corner');this._topFillerRow.createChild('td','corner').classList.add('top-filler-td');this._bottomFillerRow.createChild('td','corner').classList.add('bottom-filler-td');this._headerTableColumnGroup.createChild('col','corner');this._dataTableColumnGroup.createChild('col','corner');}
setVerticalPadding(top,bottom){this._topFillerRow.style.height=top+'px';if(top||bottom)
this._bottomFillerRow.style.height=bottom+'px';else
this._bottomFillerRow.style.height='auto';this.dispatchEventToListeners(UI.DataGrid.Events.PaddingChanged);}
setRootNode(rootNode){if(this._rootNode){this._rootNode.removeChildren();this._rootNode.dataGrid=null;this._rootNode._isRoot=false;}
this._rootNode=rootNode;rootNode._isRoot=true;rootNode.hasChildren=false;rootNode._expanded=true;rootNode._revealed=true;rootNode.selectable=false;rootNode.dataGrid=this;}
rootNode(){return this._rootNode;}
_ondblclick(event){if(this._editing||this._editingNode)
return;var columnId=this.columnIdFromNode((event.target));if(!columnId||!this._columns[columnId].editable)
return;this._startEditing((event.target));}
_startEditingColumnOfDataGridNode(node,cellIndex){this._editing=true;this._editingNode=node;this._editingNode.select();var element=this._editingNode._element.children[cellIndex];UI.InplaceEditor.startEditing(element,this._startEditingConfig(element));element.getComponentSelection().setBaseAndExtent(element,0,element,1);}
_startEditing(target){var element=(target.enclosingNodeOrSelfWithNodeName('td'));if(!element)
return;this._editingNode=this.dataGridNodeFromNode(target);if(!this._editingNode){if(!this.creationNode)
return;this._editingNode=this.creationNode;}
if(this._editingNode.isCreationNode){this._startEditingColumnOfDataGridNode(this._editingNode,this._nextEditableColumn(-1));return;}
this._editing=true;UI.InplaceEditor.startEditing(element,this._startEditingConfig(element));element.getComponentSelection().setBaseAndExtent(element,0,element,1);}
renderInline(){this.element.classList.add('inline');this._cornerWidth=0;this._inline=true;this.updateWidths();}
_startEditingConfig(element){return new UI.InplaceEditor.Config(this._editingCommitted.bind(this),this._editingCancelled.bind(this),element.textContent);}
_editingCommitted(element,newText,oldText,context,moveDirection){var columnId=this.columnIdFromNode(element);if(!columnId){this._editingCancelled(element);return;}
var column=this._columns[columnId];var cellIndex=this._visibleColumnsArray.indexOf(column);var textBeforeEditing=(this._editingNode.data[columnId]);var currentEditingNode=this._editingNode;function moveToNextIfNeeded(wasChange){if(!moveDirection)
return;if(moveDirection==='forward'){var firstEditableColumn=this._nextEditableColumn(-1);if(currentEditingNode.isCreationNode&&cellIndex===firstEditableColumn&&!wasChange)
return;var nextEditableColumn=this._nextEditableColumn(cellIndex);if(nextEditableColumn!==-1){this._startEditingColumnOfDataGridNode(currentEditingNode,nextEditableColumn);return;}
var nextDataGridNode=currentEditingNode.traverseNextNode(true,null,true);if(nextDataGridNode){this._startEditingColumnOfDataGridNode(nextDataGridNode,firstEditableColumn);return;}
if(currentEditingNode.isCreationNode&&wasChange){this.addCreationNode(false);this._startEditingColumnOfDataGridNode(this.creationNode,firstEditableColumn);return;}
return;}
if(moveDirection==='backward'){var prevEditableColumn=this._nextEditableColumn(cellIndex,true);if(prevEditableColumn!==-1){this._startEditingColumnOfDataGridNode(currentEditingNode,prevEditableColumn);return;}
var lastEditableColumn=this._nextEditableColumn(this._visibleColumnsArray.length,true);var nextDataGridNode=currentEditingNode.traversePreviousNode(true,true);if(nextDataGridNode)
this._startEditingColumnOfDataGridNode(nextDataGridNode,lastEditableColumn);return;}}
if(textBeforeEditing===newText){this._editingCancelled(element);moveToNextIfNeeded.call(this,false);return;}
this._editingNode.data[columnId]=newText;this._editCallback(this._editingNode,columnId,textBeforeEditing,newText);if(this._editingNode.isCreationNode)
this.addCreationNode(false);this._editingCancelled(element);moveToNextIfNeeded.call(this,true);}
_editingCancelled(element){this._editing=false;this._editingNode=null;}
_nextEditableColumn(cellIndex,moveBackward){var increment=moveBackward?-1:1;var columns=this._visibleColumnsArray;for(var i=cellIndex+increment;(i>=0)&&(i<columns.length);i+=increment){if(columns[i].editable)
return i;}
return-1;}
sortColumnId(){if(!this._sortColumnCell)
return null;return this._sortColumnCell[UI.DataGrid._columnIdSymbol];}
sortOrder(){if(!this._sortColumnCell||this._sortColumnCell.classList.contains(UI.DataGrid.Order.Ascending))
return UI.DataGrid.Order.Ascending;if(this._sortColumnCell.classList.contains(UI.DataGrid.Order.Descending))
return UI.DataGrid.Order.Descending;return null;}
isSortOrderAscending(){return!this._sortColumnCell||this._sortColumnCell.classList.contains(UI.DataGrid.Order.Ascending);}
_autoSizeWidths(widths,minPercent,maxPercent){if(minPercent)
minPercent=Math.min(minPercent,Math.floor(100/widths.length));var totalWidth=0;for(var i=0;i<widths.length;++i)
totalWidth+=widths[i];var totalPercentWidth=0;for(var i=0;i<widths.length;++i){var width=Math.round(100*widths[i]/totalWidth);if(minPercent&&width<minPercent)
width=minPercent;else if(maxPercent&&width>maxPercent)
width=maxPercent;totalPercentWidth+=width;widths[i]=width;}
var recoupPercent=totalPercentWidth-100;while(minPercent&&recoupPercent>0){for(var i=0;i<widths.length;++i){if(widths[i]>minPercent){--widths[i];--recoupPercent;if(!recoupPercent)
break;}}}
while(maxPercent&&recoupPercent<0){for(var i=0;i<widths.length;++i){if(widths[i]<maxPercent){++widths[i];++recoupPercent;if(!recoupPercent)
break;}}}
return widths;}
autoSizeColumns(minPercent,maxPercent,maxDescentLevel){var widths=[];for(var i=0;i<this._columnsArray.length;++i)
widths.push((this._columnsArray[i].title||'').length);maxDescentLevel=maxDescentLevel||0;var children=this._enumerateChildren(this._rootNode,[],maxDescentLevel+1);for(var i=0;i<children.length;++i){var node=children[i];for(var j=0;j<this._columnsArray.length;++j){var text=node.data[this._columnsArray[j].id];if(text.length>widths[j])
widths[j]=text.length;}}
widths=this._autoSizeWidths(widths,minPercent,maxPercent);for(var i=0;i<this._columnsArray.length;++i)
this._columnsArray[i].weight=widths[i];this._columnWidthsInitialized=false;this.updateWidths();}
_enumerateChildren(rootNode,result,maxLevel){if(!rootNode._isRoot)
result.push(rootNode);if(!maxLevel)
return[];for(var i=0;i<rootNode.children.length;++i)
this._enumerateChildren(rootNode.children[i],result,maxLevel-1);return result;}
onResize(){this.updateWidths();}
updateWidths(){if(!this._columnWidthsInitialized&&this.element.offsetWidth){var headerTableColumns=this._headerTableColumnGroup.children;var tableWidth=this.element.offsetWidth-this._cornerWidth;var cells=this._headerTableBody.rows[0].cells;var numColumns=cells.length-1;for(var i=0;i<numColumns;i++){var column=this._visibleColumnsArray[i];if(!column.weight)
column.weight=100*cells[i].offsetWidth/tableWidth;}
this._columnWidthsInitialized=true;}
this._applyColumnWeights();}
setName(name){this._columnWeightsSetting=Common.settings.createSetting('dataGrid-'+name+'-columnWeights',{});this._loadColumnWeights();}
_loadColumnWeights(){if(!this._columnWeightsSetting)
return;var weights=this._columnWeightsSetting.get();for(var i=0;i<this._columnsArray.length;++i){var column=this._columnsArray[i];var weight=weights[column.id];if(weight)
column.weight=weight;}
this._applyColumnWeights();}
_saveColumnWeights(){if(!this._columnWeightsSetting)
return;var weights={};for(var i=0;i<this._columnsArray.length;++i){var column=this._columnsArray[i];weights[column.id]=column.weight;}
this._columnWeightsSetting.set(weights);}
wasShown(){this._loadColumnWeights();}
willHide(){}
_applyColumnWeights(){var tableWidth=this.element.offsetWidth-this._cornerWidth;if(tableWidth<=0)
return;var sumOfWeights=0.0;var fixedColumnWidths=[];for(var i=0;i<this._visibleColumnsArray.length;++i){var column=this._visibleColumnsArray[i];if(column.fixedWidth){var width=this._headerTableColumnGroup.children[i][UI.DataGrid._preferredWidthSymbol]||this._headerTableBody.rows[0].cells[i].offsetWidth;fixedColumnWidths[i]=width;tableWidth-=width;}else{sumOfWeights+=this._visibleColumnsArray[i].weight;}}
var sum=0;var lastOffset=0;for(var i=0;i<this._visibleColumnsArray.length;++i){var column=this._visibleColumnsArray[i];var width;if(column.fixedWidth){width=fixedColumnWidths[i];}else{sum+=column.weight;var offset=(sum*tableWidth/sumOfWeights)|0;width=offset-lastOffset;lastOffset=offset;}
this._setPreferredWidth(i,width);}
this._positionResizers();}
setColumnsVisiblity(columnsVisibility){this._visibleColumnsArray=[];for(var i=0;i<this._columnsArray.length;++i){var column=this._columnsArray[i];if(columnsVisibility[column.id])
this._visibleColumnsArray.push(column);}
this._refreshHeader();this._applyColumnWeights();var nodes=this._enumerateChildren(this.rootNode(),[],-1);for(var i=0;i<nodes.length;++i)
nodes[i].refresh();}
get scrollContainer(){return this._scrollContainer;}
_positionResizers(){var headerTableColumns=this._headerTableColumnGroup.children;var numColumns=headerTableColumns.length-1;var left=[];var resizers=this._resizers;while(resizers.length>numColumns-1)
resizers.pop().remove();for(var i=0;i<numColumns-1;i++){left[i]=(left[i-1]||0)+this._headerTableBody.rows[0].cells[i].offsetWidth;}
for(var i=0;i<numColumns-1;i++){var resizer=resizers[i];if(!resizer){resizer=createElement('div');resizer.__index=i;resizer.classList.add('data-grid-resizer');UI.installDragHandle(resizer,this._startResizerDragging.bind(this),this._resizerDragging.bind(this),this._endResizerDragging.bind(this),'col-resize');this.element.appendChild(resizer);resizers.push(resizer);}
if(resizer.__position!==left[i]){resizer.__position=left[i];resizer.style.left=left[i]+'px';}}}
addCreationNode(hasChildren){if(this.creationNode)
this.creationNode.makeNormal();var emptyData={};for(var column in this._columns)
emptyData[column]=null;this.creationNode=new UI.CreationDataGridNode(emptyData,hasChildren);this.rootNode().appendChild(this.creationNode);}
_keyDown(event){if(!this.selectedNode||event.shiftKey||event.metaKey||event.ctrlKey||this._editing)
return;var handled=false;var nextSelectedNode;if(event.key==='ArrowUp'&&!event.altKey){nextSelectedNode=this.selectedNode.traversePreviousNode(true);while(nextSelectedNode&&!nextSelectedNode.selectable)
nextSelectedNode=nextSelectedNode.traversePreviousNode(true);handled=nextSelectedNode?true:false;}else if(event.key==='ArrowDown'&&!event.altKey){nextSelectedNode=this.selectedNode.traverseNextNode(true);while(nextSelectedNode&&!nextSelectedNode.selectable)
nextSelectedNode=nextSelectedNode.traverseNextNode(true);handled=nextSelectedNode?true:false;}else if(event.key==='ArrowLeft'){if(this.selectedNode.expanded){if(event.altKey)
this.selectedNode.collapseRecursively();else
this.selectedNode.collapse();handled=true;}else if(this.selectedNode.parent&&!this.selectedNode.parent._isRoot){handled=true;if(this.selectedNode.parent.selectable){nextSelectedNode=this.selectedNode.parent;handled=nextSelectedNode?true:false;}else if(this.selectedNode.parent){this.selectedNode.parent.collapse();}}}else if(event.key==='ArrowRight'){if(!this.selectedNode.revealed){this.selectedNode.reveal();handled=true;}else if(this.selectedNode.hasChildren){handled=true;if(this.selectedNode.expanded){nextSelectedNode=this.selectedNode.children[0];handled=nextSelectedNode?true:false;}else{if(event.altKey)
this.selectedNode.expandRecursively();else
this.selectedNode.expand();}}}else if(event.keyCode===8||event.keyCode===46){if(this._deleteCallback){handled=true;this._deleteCallback(this.selectedNode);}}else if(isEnterKey(event)){if(this._editCallback){handled=true;this._startEditing(this.selectedNode._element.children[this._nextEditableColumn(-1)]);}}
if(nextSelectedNode){nextSelectedNode.reveal();nextSelectedNode.select();}
if(handled)
event.consume(true);}
updateSelectionBeforeRemoval(root,onlyAffectsSubtree){var ancestor=this.selectedNode;while(ancestor&&ancestor!==root)
ancestor=ancestor.parent;if(!ancestor)
return;var nextSelectedNode;for(ancestor=root;ancestor&&!ancestor.nextSibling;ancestor=ancestor.parent){}
if(ancestor)
nextSelectedNode=ancestor.nextSibling;while(nextSelectedNode&&!nextSelectedNode.selectable)
nextSelectedNode=nextSelectedNode.traverseNextNode(true);if(!nextSelectedNode||nextSelectedNode.isCreationNode){nextSelectedNode=root.traversePreviousNode(true);while(nextSelectedNode&&!nextSelectedNode.selectable)
nextSelectedNode=nextSelectedNode.traversePreviousNode(true);}
if(nextSelectedNode){nextSelectedNode.reveal();nextSelectedNode.select();}else{this.selectedNode.deselect();}}
dataGridNodeFromNode(target){var rowElement=target.enclosingNodeOrSelfWithNodeName('tr');return rowElement&&rowElement._dataGridNode;}
columnIdFromNode(target){var cellElement=target.enclosingNodeOrSelfWithNodeName('td');return cellElement&&cellElement[UI.DataGrid._columnIdSymbol];}
_clickInHeaderCell(event){var cell=event.target.enclosingNodeOrSelfWithNodeName('th');if(!cell||(cell[UI.DataGrid._columnIdSymbol]===undefined)||!cell.classList.contains('sortable'))
return;var sortOrder=UI.DataGrid.Order.Ascending;if((cell===this._sortColumnCell)&&this.isSortOrderAscending())
sortOrder=UI.DataGrid.Order.Descending;if(this._sortColumnCell)
this._sortColumnCell.classList.remove(UI.DataGrid.Order.Ascending,UI.DataGrid.Order.Descending);this._sortColumnCell=cell;cell.classList.add(sortOrder);this.dispatchEventToListeners(UI.DataGrid.Events.SortingChanged);}
markColumnAsSortedBy(columnId,sortOrder){if(this._sortColumnCell)
this._sortColumnCell.classList.remove(UI.DataGrid.Order.Ascending,UI.DataGrid.Order.Descending);this._sortColumnCell=this._headerTableHeaders[columnId];this._sortColumnCell.classList.add(sortOrder);}
headerTableHeader(columnId){return this._headerTableHeaders[columnId];}
_mouseDownInDataTable(event){var target=(event.target);var gridNode=this.dataGridNodeFromNode(target);if(!gridNode||!gridNode.selectable||gridNode.isEventWithinDisclosureTriangle(event))
return;var columnId=this.columnIdFromNode(target);if(columnId&&this._columns[columnId].nonSelectable)
return;if(event.metaKey){if(gridNode.selected)
gridNode.deselect();else
gridNode.select();}else{gridNode.select();}}
setHeaderContextMenuCallback(callback){this._headerContextMenuCallback=callback;}
setRowContextMenuCallback(callback){this._rowContextMenuCallback=callback;}
_contextMenu(event){var contextMenu=new UI.ContextMenu(event);var target=(event.target);if(target.isSelfOrDescendant(this._headerTableBody)){if(this._headerContextMenuCallback)
this._headerContextMenuCallback(contextMenu);return;}
var gridNode=this.dataGridNodeFromNode(target);if(this._refreshCallback&&(!gridNode||gridNode!==this.creationNode))
contextMenu.appendItem(Common.UIString('Refresh'),this._refreshCallback.bind(this));if(gridNode&&gridNode.selectable&&!gridNode.isEventWithinDisclosureTriangle(event)){if(this._editCallback){if(gridNode===this.creationNode){contextMenu.appendItem(Common.UIString.capitalize('Add ^new'),this._startEditing.bind(this,target));}else{var columnId=this.columnIdFromNode(target);if(columnId&&this._columns[columnId].editable){contextMenu.appendItem(Common.UIString('Edit "%s"',this._columns[columnId].title),this._startEditing.bind(this,target));}}}
if(this._deleteCallback&&gridNode!==this.creationNode)
contextMenu.appendItem(Common.UIString.capitalize('Delete'),this._deleteCallback.bind(this,gridNode));if(this._rowContextMenuCallback)
this._rowContextMenuCallback(contextMenu,gridNode);}
contextMenu.show();}
_clickInDataTable(event){var gridNode=this.dataGridNodeFromNode((event.target));if(!gridNode||!gridNode.hasChildren||!gridNode.isEventWithinDisclosureTriangle(event))
return;if(gridNode.expanded){if(event.altKey)
gridNode.collapseRecursively();else
gridNode.collapse();}else{if(event.altKey)
gridNode.expandRecursively();else
gridNode.expand();}}
setResizeMethod(method){this._resizeMethod=method;}
_startResizerDragging(event){this._currentResizer=event.target;return true;}
_endResizerDragging(){this._currentResizer=null;this._saveColumnWeights();}
_resizerDragging(event){var resizer=this._currentResizer;if(!resizer)
return;var dragPoint=event.clientX-this.element.totalOffsetLeft();var firstRowCells=this._headerTableBody.rows[0].cells;var leftEdgeOfPreviousColumn=0;var leftCellIndex=resizer.__index;var rightCellIndex=leftCellIndex+1;for(var i=0;i<leftCellIndex;i++)
leftEdgeOfPreviousColumn+=firstRowCells[i].offsetWidth;if(this._resizeMethod===UI.DataGrid.ResizeMethod.Last){rightCellIndex=this._resizers.length;}else if(this._resizeMethod===UI.DataGrid.ResizeMethod.First){leftEdgeOfPreviousColumn+=firstRowCells[leftCellIndex].offsetWidth-firstRowCells[0].offsetWidth;leftCellIndex=0;}
var rightEdgeOfNextColumn=leftEdgeOfPreviousColumn+firstRowCells[leftCellIndex].offsetWidth+firstRowCells[rightCellIndex].offsetWidth;var leftMinimum=leftEdgeOfPreviousColumn+UI.DataGrid.ColumnResizePadding;var rightMaximum=rightEdgeOfNextColumn-UI.DataGrid.ColumnResizePadding;if(leftMinimum>rightMaximum)
return;dragPoint=Number.constrain(dragPoint,leftMinimum,rightMaximum);var position=(dragPoint-UI.DataGrid.CenterResizerOverBorderAdjustment);resizer.__position=position;resizer.style.left=position+'px';this._setPreferredWidth(leftCellIndex,dragPoint-leftEdgeOfPreviousColumn);this._setPreferredWidth(rightCellIndex,rightEdgeOfNextColumn-dragPoint);var leftColumn=this._visibleColumnsArray[leftCellIndex];var rightColumn=this._visibleColumnsArray[rightCellIndex];if(leftColumn.weight||rightColumn.weight){var sumOfWeights=leftColumn.weight+rightColumn.weight;var delta=rightEdgeOfNextColumn-leftEdgeOfPreviousColumn;leftColumn.weight=(dragPoint-leftEdgeOfPreviousColumn)*sumOfWeights/delta;rightColumn.weight=(rightEdgeOfNextColumn-dragPoint)*sumOfWeights/delta;}
this._positionResizers();event.preventDefault();}
_setPreferredWidth(columnIndex,width){var pxWidth=width+'px';this._headerTableColumnGroup.children[columnIndex][UI.DataGrid._preferredWidthSymbol]=width;this._headerTableColumnGroup.children[columnIndex].style.width=pxWidth;this._dataTableColumnGroup.children[columnIndex].style.width=pxWidth;}
columnOffset(columnId){if(!this.element.offsetWidth)
return 0;for(var i=1;i<this._visibleColumnsArray.length;++i){if(columnId===this._visibleColumnsArray[i].id){if(this._resizers[i-1])
return this._resizers[i-1].__position;}}
return 0;}
asWidget(){if(!this._dataGridWidget)
this._dataGridWidget=new UI.DataGridWidget(this);return this._dataGridWidget;}};UI.DataGrid.CornerWidth=14;UI.DataGrid.ColumnDescriptor;UI.DataGrid.Events={SelectedNode:Symbol('SelectedNode'),DeselectedNode:Symbol('DeselectedNode'),SortingChanged:Symbol('SortingChanged'),PaddingChanged:Symbol('PaddingChanged')};UI.DataGrid.Order={Ascending:'sort-ascending',Descending:'sort-descending'};UI.DataGrid.Align={Center:'center',Right:'right'};UI.DataGrid._preferredWidthSymbol=Symbol('preferredWidth');UI.DataGrid._columnIdSymbol=Symbol('columnId');UI.DataGrid.ColumnResizePadding=24;UI.DataGrid.CenterResizerOverBorderAdjustment=3;UI.DataGrid.ResizeMethod={Nearest:'nearest',First:'first',Last:'last'};UI.DataGridNode=class extends Common.Object{constructor(data,hasChildren){super();this._element=null;this._expanded=false;this._selected=false;this._depth;this._revealed;this._attached=false;this._savedPosition=null;this._shouldRefreshChildren=true;this._data=data||{};this.hasChildren=hasChildren||false;this.children=[];this.dataGrid=null;this.parent=null;this.previousSibling=null;this.nextSibling=null;this.disclosureToggleWidth=10;this.selectable=true;this._isRoot=false;}
element(){if(!this._element){this.createElement();this.createCells();}
return(this._element);}
createElement(){this._element=createElement('tr');this._element._dataGridNode=this;if(this.hasChildren)
this._element.classList.add('parent');if(this.expanded)
this._element.classList.add('expanded');if(this.selected)
this._element.classList.add('selected');if(this.revealed)
this._element.classList.add('revealed');}
createCells(){this._element.removeChildren();var columnsArray=this.dataGrid._visibleColumnsArray;for(var i=0;i<columnsArray.length;++i)
this._element.appendChild(this.createCell(columnsArray[i].id));this._element.appendChild(this._createTDWithClass('corner'));}
get data(){return this._data;}
set data(x){this._data=x||{};this.refresh();}
get revealed(){if(this._revealed!==undefined)
return this._revealed;var currentAncestor=this.parent;while(currentAncestor&&!currentAncestor._isRoot){if(!currentAncestor.expanded){this._revealed=false;return false;}
currentAncestor=currentAncestor.parent;}
this._revealed=true;return true;}
set revealed(x){if(this._revealed===x)
return;this._revealed=x;if(this._element)
this._element.classList.toggle('revealed',this._revealed);for(var i=0;i<this.children.length;++i)
this.children[i].revealed=x&&this.expanded;}
get hasChildren(){return this._hasChildren;}
set hasChildren(x){if(this._hasChildren===x)
return;this._hasChildren=x;if(!this._element)
return;this._element.classList.toggle('parent',this._hasChildren);this._element.classList.toggle('expanded',this._hasChildren&&this.expanded);}
get depth(){if(this._depth!==undefined)
return this._depth;if(this.parent&&!this.parent._isRoot)
this._depth=this.parent.depth+1;else
this._depth=0;return this._depth;}
get leftPadding(){return this.depth*this.dataGrid.indentWidth;}
get shouldRefreshChildren(){return this._shouldRefreshChildren;}
set shouldRefreshChildren(x){this._shouldRefreshChildren=x;if(x&&this.expanded)
this.expand();}
get selected(){return this._selected;}
set selected(x){if(x)
this.select();else
this.deselect();}
get expanded(){return this._expanded;}
set expanded(x){if(x)
this.expand();else
this.collapse();}
refresh(){if(!this.dataGrid)
this._element=null;if(!this._element)
return;this.createCells();}
_createTDWithClass(className){var cell=createElementWithClass('td',className);var cellClass=this.dataGrid._cellClass;if(cellClass)
cell.classList.add(cellClass);return cell;}
createTD(columnId){var cell=this._createTDWithClass(columnId+'-column');cell[UI.DataGrid._columnIdSymbol]=columnId;var alignment=this.dataGrid._columns[columnId].align;if(alignment)
cell.classList.add(alignment);if(columnId===this.dataGrid.disclosureColumnId){cell.classList.add('disclosure');if(this.leftPadding)
cell.style.setProperty('padding-left',this.leftPadding+'px');}
return cell;}
createCell(columnId){var cell=this.createTD(columnId);var data=this.data[columnId];if(data instanceof Node){cell.appendChild(data);}else{cell.textContent=data;if(this.dataGrid._columns[columnId].longText)
cell.title=data;}
return cell;}
nodeSelfHeight(){return 16;}
appendChild(child){this.insertChild(child,this.children.length);}
insertChild(child,index){if(!child)
throw'insertChild: Node can\'t be undefined or null.';if(child.parent===this){var currentIndex=this.children.indexOf(child);if(currentIndex<0)
console.assert(false,'Inconsistent DataGrid state');if(currentIndex===index)
return;if(currentIndex<index)
--index;}
child.remove();this.children.splice(index,0,child);this.hasChildren=true;child.parent=this;child.dataGrid=this.dataGrid;child.recalculateSiblings(index);child._depth=undefined;child._revealed=undefined;child._attached=false;child._shouldRefreshChildren=true;var current=child.children[0];while(current){current.dataGrid=this.dataGrid;current._depth=undefined;current._revealed=undefined;current._attached=false;current._shouldRefreshChildren=true;current=current.traverseNextNode(false,child,true);}
if(this.expanded)
child._attach();if(!this.revealed)
child.revealed=false;}
remove(){if(this.parent)
this.parent.removeChild(this);}
removeChild(child){if(!child)
throw'removeChild: Node can\'t be undefined or null.';if(child.parent!==this)
throw'removeChild: Node is not a child of this node.';if(this.dataGrid)
this.dataGrid.updateSelectionBeforeRemoval(child,false);child._detach();this.children.remove(child,true);if(child.previousSibling)
child.previousSibling.nextSibling=child.nextSibling;if(child.nextSibling)
child.nextSibling.previousSibling=child.previousSibling;child.dataGrid=null;child.parent=null;child.nextSibling=null;child.previousSibling=null;if(this.children.length<=0)
this.hasChildren=false;}
removeChildren(){if(this.dataGrid)
this.dataGrid.updateSelectionBeforeRemoval(this,true);for(var i=0;i<this.children.length;++i){var child=this.children[i];child._detach();child.dataGrid=null;child.parent=null;child.nextSibling=null;child.previousSibling=null;}
this.children=[];this.hasChildren=false;}
recalculateSiblings(myIndex){if(!this.parent)
return;var previousChild=this.parent.children[myIndex-1]||null;if(previousChild)
previousChild.nextSibling=this;this.previousSibling=previousChild;var nextChild=this.parent.children[myIndex+1]||null;if(nextChild)
nextChild.previousSibling=this;this.nextSibling=nextChild;}
collapse(){if(this._isRoot)
return;if(this._element)
this._element.classList.remove('expanded');this._expanded=false;for(var i=0;i<this.children.length;++i)
this.children[i].revealed=false;}
collapseRecursively(){var item=this;while(item){if(item.expanded)
item.collapse();item=item.traverseNextNode(false,this,true);}}
populate(){}
expand(){if(!this.hasChildren||this.expanded)
return;if(this._isRoot)
return;if(this.revealed&&!this._shouldRefreshChildren){for(var i=0;i<this.children.length;++i)
this.children[i].revealed=true;}
if(this._shouldRefreshChildren){for(var i=0;i<this.children.length;++i)
this.children[i]._detach();this.populate();if(this._attached){for(var i=0;i<this.children.length;++i){var child=this.children[i];if(this.revealed)
child.revealed=true;child._attach();}}
this._shouldRefreshChildren=false;}
if(this._element)
this._element.classList.add('expanded');this._expanded=true;}
expandRecursively(){var item=this;while(item){item.expand();item=item.traverseNextNode(false,this);}}
reveal(){if(this._isRoot)
return;var currentAncestor=this.parent;while(currentAncestor&&!currentAncestor._isRoot){if(!currentAncestor.expanded)
currentAncestor.expand();currentAncestor=currentAncestor.parent;}
this.element().scrollIntoViewIfNeeded(false);}
select(supressSelectedEvent){if(!this.dataGrid||!this.selectable||this.selected)
return;if(this.dataGrid.selectedNode)
this.dataGrid.selectedNode.deselect();this._selected=true;this.dataGrid.selectedNode=this;if(this._element)
this._element.classList.add('selected');if(!supressSelectedEvent)
this.dataGrid.dispatchEventToListeners(UI.DataGrid.Events.SelectedNode);}
revealAndSelect(){if(this._isRoot)
return;this.reveal();this.select();}
deselect(supressDeselectedEvent){if(!this.dataGrid||this.dataGrid.selectedNode!==this||!this.selected)
return;this._selected=false;this.dataGrid.selectedNode=null;if(this._element)
this._element.classList.remove('selected');if(!supressDeselectedEvent)
this.dataGrid.dispatchEventToListeners(UI.DataGrid.Events.DeselectedNode);}
traverseNextNode(skipHidden,stayWithin,dontPopulate,info){if(!dontPopulate&&this.hasChildren)
this.populate();if(info)
info.depthChange=0;var node=(!skipHidden||this.revealed)?this.children[0]:null;if(node&&(!skipHidden||this.expanded)){if(info)
info.depthChange=1;return node;}
if(this===stayWithin)
return null;node=(!skipHidden||this.revealed)?this.nextSibling:null;if(node)
return node;node=this;while(node&&!node._isRoot&&!((!skipHidden||node.revealed)?node.nextSibling:null)&&node.parent!==stayWithin){if(info)
info.depthChange-=1;node=node.parent;}
if(!node)
return null;return(!skipHidden||node.revealed)?node.nextSibling:null;}
traversePreviousNode(skipHidden,dontPopulate){var node=(!skipHidden||this.revealed)?this.previousSibling:null;if(!dontPopulate&&node&&node.hasChildren)
node.populate();while(node&&((!skipHidden||(node.revealed&&node.expanded))?node.children[node.children.length-1]:null)){if(!dontPopulate&&node.hasChildren)
node.populate();node=((!skipHidden||(node.revealed&&node.expanded))?node.children[node.children.length-1]:null);}
if(node)
return node;if(!this.parent||this.parent._isRoot)
return null;return this.parent;}
isEventWithinDisclosureTriangle(event){if(!this.hasChildren)
return false;var cell=event.target.enclosingNodeOrSelfWithNodeName('td');if(!cell||!cell.classList.contains('disclosure'))
return false;var left=cell.totalOffsetLeft()+this.leftPadding;return event.pageX>=left&&event.pageX<=left+this.disclosureToggleWidth;}
_attach(){if(!this.dataGrid||this._attached)
return;this._attached=true;var previousNode=this.traversePreviousNode(true,true);var previousElement=previousNode?previousNode.element():this.dataGrid._topFillerRow;this.dataGrid.dataTableBody.insertBefore(this.element(),previousElement.nextSibling);if(this.expanded){for(var i=0;i<this.children.length;++i)
this.children[i]._attach();}}
_detach(){if(!this._attached)
return;this._attached=false;if(this._element)
this._element.remove();for(var i=0;i<this.children.length;++i)
this.children[i]._detach();this.wasDetached();}
wasDetached(){}
savePosition(){if(this._savedPosition)
return;if(!this.parent)
throw'savePosition: Node must have a parent.';this._savedPosition={parent:this.parent,index:this.parent.children.indexOf(this)};}
restorePosition(){if(!this._savedPosition)
return;if(this.parent!==this._savedPosition.parent)
this._savedPosition.parent.insertChild(this,this._savedPosition.index);this._savedPosition=null;}};UI.CreationDataGridNode=class extends UI.DataGridNode{constructor(data,hasChildren){super(data,hasChildren);this.isCreationNode=true;}
makeNormal(){this.isCreationNode=false;}};UI.DataGridWidget=class extends UI.VBox{constructor(dataGrid){super();this._dataGrid=dataGrid;this.element.appendChild(dataGrid.element);}
wasShown(){this._dataGrid.wasShown();}
willHide(){this._dataGrid.willHide();}
onResize(){this._dataGrid.onResize();}
elementsToRestoreScrollPositionsFor(){return[this._dataGrid._scrollContainer];}
detachChildWidgets(){super.detachChildWidgets();for(var dataGrid of this._dataGrids)
this.element.removeChild(dataGrid.element);this._dataGrids=[];}};;UI.ViewportDataGrid=class extends UI.DataGrid{constructor(columnsArray,editCallback,deleteCallback,refreshCallback){super(columnsArray,editCallback,deleteCallback,refreshCallback);this._onScrollBound=this._onScroll.bind(this);this._scrollContainer.addEventListener('scroll',this._onScrollBound,true);this._scrollContainer.addEventListener('mousewheel',this._onWheel.bind(this),true);this._visibleNodes=[];this._flatNodes=null;this._inline=false;this._wheelTarget=null;this._hiddenWheelTarget=null;this._stickToBottom=false;this._atBottom=true;this._lastScrollTop=0;this.setRootNode(new UI.ViewportDataGridNode());}
setScrollContainer(scrollContainer){this._scrollContainer.removeEventListener('scroll',this._onScrollBound,true);this._scrollContainer=scrollContainer;this._scrollContainer.addEventListener('scroll',this._onScrollBound,true);}
onResize(){if(this._stickToBottom&&this._atBottom)
this._scrollContainer.scrollTop=this._scrollContainer.scrollHeight-this._scrollContainer.clientHeight;this.scheduleUpdate();super.onResize();}
setStickToBottom(stick){this._stickToBottom=stick;}
_onWheel(event){this._wheelTarget=event.target?event.target.enclosingNodeOrSelfWithNodeName('tr'):null;}
_onScroll(event){this._atBottom=this._scrollContainer.isScrolledToBottom();if(this._lastScrollTop!==this._scrollContainer.scrollTop)
this.scheduleUpdate();}
scheduleUpdateStructure(){this._flatNodes=null;this.scheduleUpdate();}
scheduleUpdate(){if(this._updateAnimationFrameId)
return;this._updateAnimationFrameId=this.element.window().requestAnimationFrame(this._update.bind(this));}
updateInstantlyForTests(){if(!this._updateAnimationFrameId)
return;this.element.window().cancelAnimationFrame(this._updateAnimationFrameId);this._update();}
renderInline(){this._inline=true;super.renderInline();this._update();}
_flatNodesList(){if(this._flatNodes)
return this._flatNodes;var flatNodes=[];var children=[this._rootNode.children];var counters=[0];var depth=0;while(depth>=0){var node=children[depth][counters[depth]++];if(!node){depth--;continue;}
flatNodes.push(node);node.setDepth(depth);if(node._expanded&&node.children.length){depth++;children[depth]=node.children;counters[depth]=0;}}
this._flatNodes=flatNodes;return this._flatNodes;}
_calculateVisibleNodes(clientHeight,scrollTop){var nodes=this._flatNodesList();if(this._inline)
return{topPadding:0,bottomPadding:0,contentHeight:0,visibleNodes:nodes,offset:0};var size=nodes.length;var i=0;var y=0;for(;i<size&&y+nodes[i].nodeSelfHeight()<scrollTop;++i)
y+=nodes[i].nodeSelfHeight();var start=i;var topPadding=y;for(;i<size&&y<scrollTop+clientHeight;++i)
y+=nodes[i].nodeSelfHeight();var end=i;var bottomPadding=0;for(;i<size;++i)
bottomPadding+=nodes[i].nodeSelfHeight();return{topPadding:topPadding,bottomPadding:bottomPadding,contentHeight:y-topPadding,visibleNodes:nodes.slice(start,end),offset:start};}
_contentHeight(){var nodes=this._flatNodesList();var result=0;for(var i=0,size=nodes.length;i<size;++i)
result+=nodes[i].nodeSelfHeight();return result;}
_update(){delete this._updateAnimationFrameId;var clientHeight=this._scrollContainer.clientHeight;var scrollTop=this._scrollContainer.scrollTop;var currentScrollTop=scrollTop;var maxScrollTop=Math.max(0,this._contentHeight()-clientHeight);if(this._stickToBottom&&this._atBottom)
scrollTop=maxScrollTop;scrollTop=Math.min(maxScrollTop,scrollTop);this._atBottom=scrollTop===maxScrollTop;var viewportState=this._calculateVisibleNodes(clientHeight,scrollTop);var visibleNodes=viewportState.visibleNodes;var visibleNodesSet=new Set(visibleNodes);if(this._hiddenWheelTarget&&this._hiddenWheelTarget!==this._wheelTarget){this._hiddenWheelTarget.remove();this._hiddenWheelTarget=null;}
for(var i=0;i<this._visibleNodes.length;++i){var oldNode=this._visibleNodes[i];if(!visibleNodesSet.has(oldNode)&&oldNode.attached()){var element=oldNode._element;if(element===this._wheelTarget)
this._hiddenWheelTarget=oldNode.abandonElement();else
element.remove();oldNode.wasDetached();}}
var previousElement=this._topFillerRow;if(previousElement.nextSibling===this._hiddenWheelTarget)
previousElement=this._hiddenWheelTarget;var tBody=this.dataTableBody;var offset=viewportState.offset;for(var i=0;i<visibleNodes.length;++i){var node=visibleNodes[i];var element=node.element();node.willAttach();element.classList.toggle('odd',(offset+i)%2===0);tBody.insertBefore(element,previousElement.nextSibling);node.revealed=true;previousElement=element;}
this.setVerticalPadding(viewportState.topPadding,viewportState.bottomPadding);this._lastScrollTop=scrollTop;if(scrollTop!==currentScrollTop)
this._scrollContainer.scrollTop=scrollTop;var contentFits=viewportState.contentHeight<=clientHeight&&viewportState.topPadding+viewportState.bottomPadding===0;if(contentFits!==this.element.classList.contains('data-grid-fits-viewport')){this.element.classList.toggle('data-grid-fits-viewport',contentFits);this.updateWidths();}
this._visibleNodes=visibleNodes;this.dispatchEventToListeners(UI.ViewportDataGrid.Events.ViewportCalculated);}
_revealViewportNode(node){var nodes=this._flatNodesList();var index=nodes.indexOf(node);if(index===-1)
return;var fromY=0;for(var i=0;i<index;++i)
fromY+=nodes[i].nodeSelfHeight();var toY=fromY+node.nodeSelfHeight();var scrollTop=this._scrollContainer.scrollTop;if(scrollTop>fromY){scrollTop=fromY;this._atBottom=false;}else if(scrollTop+this._scrollContainer.offsetHeight<toY){scrollTop=toY-this._scrollContainer.offsetHeight;}
this._scrollContainer.scrollTop=scrollTop;}};UI.ViewportDataGrid.Events={ViewportCalculated:Symbol('ViewportCalculated')};UI.ViewportDataGridNode=class extends UI.DataGridNode{constructor(data,hasChildren){super(data,hasChildren);this._stale=false;}
element(){if(!this._element){this.createElement();this.createCells();this._stale=false;}
if(this._stale){this.createCells();this._stale=false;}
return(this._element);}
setDepth(depth){this._depth=depth;}
insertChild(child,index){if(child.parent===this){var currentIndex=this.children.indexOf(child);if(currentIndex<0)
console.assert(false,'Inconsistent DataGrid state');if(currentIndex===index)
return;if(currentIndex<index)
--index;}
child.remove();child.parent=this;child.dataGrid=this.dataGrid;if(!this.children.length)
this.hasChildren=true;this.children.splice(index,0,child);child.recalculateSiblings(index);if(this._expanded)
this.dataGrid.scheduleUpdateStructure();}
removeChild(child){if(this.dataGrid)
this.dataGrid.updateSelectionBeforeRemoval(child,false);if(child.previousSibling)
child.previousSibling.nextSibling=child.nextSibling;if(child.nextSibling)
child.nextSibling.previousSibling=child.previousSibling;if(child.parent!==this)
throw'removeChild: Node is not a child of this node.';child._unlink();this.children.remove(child,true);if(!this.children.length)
this.hasChildren=false;if(this._expanded)
this.dataGrid.scheduleUpdateStructure();}
removeChildren(){if(this.dataGrid)
this.dataGrid.updateSelectionBeforeRemoval(this,true);for(var i=0;i<this.children.length;++i)
this.children[i]._unlink();this.children=[];if(this._expanded)
this.dataGrid.scheduleUpdateStructure();}
_unlink(){if(this.attached()){this._element.remove();this.wasDetached();}
this.dataGrid=null;this.parent=null;this.nextSibling=null;this.previousSibling=null;}
collapse(){if(!this._expanded)
return;this._expanded=false;if(this._element)
this._element.classList.remove('expanded');this.dataGrid.scheduleUpdateStructure();}
expand(){if(this._expanded)
return;super.expand();this.dataGrid.scheduleUpdateStructure();}
willAttach(){}
attached(){return!!(this.dataGrid&&this._element&&this._element.parentElement);}
refresh(){if(this.attached()){this._stale=true;this.dataGrid.scheduleUpdate();}else{this._element=null;}}
abandonElement(){var result=this._element;if(result)
result.style.display='none';this._element=null;return result;}
reveal(){this.dataGrid._revealViewportNode(this);}};;UI.SortableDataGrid=class extends UI.ViewportDataGrid{constructor(columnsArray,editCallback,deleteCallback,refreshCallback){super(columnsArray,editCallback,deleteCallback,refreshCallback);this._sortingFunction=UI.SortableDataGrid.TrivialComparator;this.setRootNode(new UI.SortableDataGridNode());}
static TrivialComparator(a,b){return 0;}
static NumericComparator(columnId,a,b){var aValue=a.data[columnId];var bValue=b.data[columnId];var aNumber=Number(aValue instanceof Node?aValue.textContent:aValue);var bNumber=Number(bValue instanceof Node?bValue.textContent:bValue);return aNumber<bNumber?-1:(aNumber>bNumber?1:0);}
static StringComparator(columnId,a,b){var aValue=a.data[columnId];var bValue=b.data[columnId];var aString=aValue instanceof Node?aValue.textContent:String(aValue);var bString=bValue instanceof Node?bValue.textContent:String(bValue);return aString<bString?-1:(aString>bString?1:0);}
static Comparator(comparator,reverseMode,a,b){return reverseMode?comparator(b,a):comparator(a,b);}
static create(columnNames,values){var numColumns=columnNames.length;if(!numColumns)
return null;var columns=([]);for(var i=0;i<columnNames.length;++i)
columns.push({id:String(i),title:columnNames[i],width:columnNames[i].length,sortable:true});var nodes=[];for(var i=0;i<values.length/numColumns;++i){var data={};for(var j=0;j<columnNames.length;++j)
data[j]=values[numColumns*i+j];var node=new UI.SortableDataGridNode(data);node.selectable=false;nodes.push(node);}
var dataGrid=new UI.SortableDataGrid(columns);var length=nodes.length;var rootNode=dataGrid.rootNode();for(var i=0;i<length;++i)
rootNode.appendChild(nodes[i]);dataGrid.addEventListener(UI.DataGrid.Events.SortingChanged,sortDataGrid);function sortDataGrid(){var nodes=dataGrid.rootNode().children;var sortColumnId=dataGrid.sortColumnId();if(!sortColumnId)
return;var columnIsNumeric=true;for(var i=0;i<nodes.length;i++){var value=nodes[i].data[sortColumnId];if(isNaN(value instanceof Node?value.textContent:value)){columnIsNumeric=false;break;}}
var comparator=columnIsNumeric?UI.SortableDataGrid.NumericComparator:UI.SortableDataGrid.StringComparator;dataGrid.sortNodes(comparator.bind(null,sortColumnId),!dataGrid.isSortOrderAscending());}
return dataGrid;}
insertChild(node){var root=(this.rootNode());root.insertChildOrdered(node);}
sortNodes(comparator,reverseMode){this._sortingFunction=UI.SortableDataGrid.Comparator.bind(null,comparator,reverseMode);this._rootNode._sortChildren(reverseMode);this.scheduleUpdateStructure();}};UI.SortableDataGrid.NodeComparator;UI.SortableDataGridNode=class extends UI.ViewportDataGridNode{constructor(data,hasChildren){super(data,hasChildren);}
insertChildOrdered(node){this.insertChild(node,this.children.upperBound(node,this.dataGrid._sortingFunction));}
_sortChildren(){this.children.sort(this.dataGrid._sortingFunction);for(var i=0;i<this.children.length;++i)
this.children[i].recalculateSiblings(i);for(var child of this.children)
child._sortChildren();}};;UI.ShowMoreDataGridNode=class extends UI.DataGridNode{constructor(callback,startPosition,endPosition,chunkSize){super({summaryRow:true},false);this._callback=callback;this._startPosition=startPosition;this._endPosition=endPosition;this._chunkSize=chunkSize;this.showNext=createElement('button');this.showNext.setAttribute('type','button');this.showNext.addEventListener('click',this._showNextChunk.bind(this),false);this.showNext.textContent=Common.UIString('Show %d before',this._chunkSize);this.showAll=createElement('button');this.showAll.setAttribute('type','button');this.showAll.addEventListener('click',this._showAll.bind(this),false);this.showLast=createElement('button');this.showLast.setAttribute('type','button');this.showLast.addEventListener('click',this._showLastChunk.bind(this),false);this.showLast.textContent=Common.UIString('Show %d after',this._chunkSize);this._updateLabels();this.selectable=false;}
_showNextChunk(){this._callback(this._startPosition,this._startPosition+this._chunkSize);}
_showAll(){this._callback(this._startPosition,this._endPosition);}
_showLastChunk(){this._callback(this._endPosition-this._chunkSize,this._endPosition);}
_updateLabels(){var totalSize=this._endPosition-this._startPosition;if(totalSize>this._chunkSize){this.showNext.classList.remove('hidden');this.showLast.classList.remove('hidden');}else{this.showNext.classList.add('hidden');this.showLast.classList.add('hidden');}
this.showAll.textContent=Common.UIString('Show all %d',totalSize);}
createCells(){this._hasCells=false;super.createCells();}
createCell(columnIdentifier){var cell=this.createTD(columnIdentifier);if(!this._hasCells){this._hasCells=true;if(this.depth)
cell.style.setProperty('padding-left',(this.depth*this.dataGrid.indentWidth)+'px');cell.appendChild(this.showNext);cell.appendChild(this.showAll);cell.appendChild(this.showLast);}
return cell;}
setStartPosition(from){this._startPosition=from;this._updateLabels();}
setEndPosition(to){this._endPosition=to;this._updateLabels();}
nodeSelfHeight(){return 32;}
dispose(){}};;UI.FilteredListWidget=class extends UI.VBox{constructor(delegate){super(true);this._renderAsTwoRows=delegate.renderAsTwoRows();this.contentElement.classList.add('filtered-list-widget');this.contentElement.addEventListener('keydown',this._onKeyDown.bind(this),true);if(delegate.renderMonospace())
this.contentElement.classList.add('monospace');this.registerRequiredCSS('ui_lazy/filteredListWidget.css');this._promptElement=this.contentElement.createChild('div','filtered-list-widget-input');this._promptElement.setAttribute('spellcheck','false');this._promptElement.setAttribute('contenteditable','plaintext-only');this._prompt=new UI.TextPrompt();this._prompt.initialize(()=>Promise.resolve([]));this._prompt.renderAsBlock();var promptProxy=this._prompt.attach(this._promptElement);promptProxy.addEventListener('input',this._onInput.bind(this),false);promptProxy.classList.add('filtered-list-widget-prompt-element');this._filteredItems=[];this._viewportControl=new UI.StaticViewportControl(this);this._itemElementsContainer=this._viewportControl.element;this._itemElementsContainer.classList.add('container');this._itemElementsContainer.addEventListener('click',this._onClick.bind(this),false);this.contentElement.appendChild(this._itemElementsContainer);this.setDefaultFocusedElement(this._promptElement);this._delegate=delegate;this._delegate.setRefreshCallback(this._itemsLoaded.bind(this));this._itemsLoaded();this._updateShowMatchingItems();this._viewportControl.refresh();this._elements=[];}
static filterRegex(query){const toEscape=String.regexSpecialCharacters();var regexString='';for(var i=0;i<query.length;++i){var c=query.charAt(i);if(toEscape.indexOf(c)!==-1)
c='\\'+c;if(i)
regexString+='[^\\0'+c+']*';regexString+=c;}
return new RegExp(regexString,'i');}
showAsDialog(){this._dialog=new UI.Dialog();this._dialog.setMaxSize(new Size(504,340));this._dialog.setPosition(undefined,22);this.show(this._dialog.element);this._dialog.show();}
_value(){return this._prompt.text().trim();}
willHide(){this._delegate.dispose();if(this._filterTimer)
clearTimeout(this._filterTimer);}
_onEnter(event){event.preventDefault();if(!this._delegate.itemCount())
return;var selectedIndex=this._shouldShowMatchingItems()&&this._selectedIndexInFiltered<this._filteredItems.length?this._filteredItems[this._selectedIndexInFiltered]:null;if(this._dialog)
this._dialog.detach();this._delegate.selectItemWithQuery(selectedIndex,this._value());}
_itemsLoaded(){if(this._loadTimeout)
return;this._loadTimeout=setTimeout(this._updateAfterItemsLoaded.bind(this),0);}
_updateAfterItemsLoaded(){delete this._loadTimeout;this._filterItems();}
_createItemElement(index){var itemElement=createElement('div');itemElement.className='filtered-list-widget-item '+(this._renderAsTwoRows?'two-rows':'one-row');itemElement._titleElement=itemElement.createChild('div','filtered-list-widget-title');itemElement._subtitleElement=itemElement.createChild('div','filtered-list-widget-subtitle');itemElement._subtitleElement.textContent='\u200B';itemElement._index=index;this._delegate.renderItem(index,this._value(),itemElement._titleElement,itemElement._subtitleElement);return itemElement;}
setQuery(query){this._prompt.setText(query);this._prompt.autoCompleteSoon(true);this._scheduleFilter();}
_tabKeyPressed(){var userEnteredText=this._prompt.text();var completion=this._delegate.autocomplete(userEnteredText);this._prompt.setText(completion);this._prompt.setDOMSelection(userEnteredText.length,completion.length);}
_itemsFilteredForTest(){}
_filterItems(){delete this._filterTimer;if(this._scoringTimer){clearTimeout(this._scoringTimer);delete this._scoringTimer;}
var query=this._delegate.rewriteQuery(this._value());this._query=query;var filterRegex=query?UI.FilteredListWidget.filterRegex(query):null;var oldSelectedAbsoluteIndex=this._selectedIndexInFiltered?this._filteredItems[this._selectedIndexInFiltered]:null;var filteredItems=[];this._selectedIndexInFiltered=0;var bestScores=[];var bestItems=[];var bestItemsToCollect=100;var minBestScore=0;var overflowItems=[];scoreItems.call(this,0);function compareIntegers(a,b){return b-a;}
function scoreItems(fromIndex){var maxWorkItems=1000;var workDone=0;for(var i=fromIndex;i<this._delegate.itemCount()&&workDone<maxWorkItems;++i){if(filterRegex&&!filterRegex.test(this._delegate.itemKeyAt(i)))
continue;var score=this._delegate.itemScoreAt(i,query);if(query)
workDone++;if(score>minBestScore||bestScores.length<bestItemsToCollect){var index=bestScores.upperBound(score,compareIntegers);bestScores.splice(index,0,score);bestItems.splice(index,0,i);if(bestScores.length>bestItemsToCollect){overflowItems.push(bestItems.peekLast());bestScores.length=bestItemsToCollect;bestItems.length=bestItemsToCollect;}
minBestScore=bestScores.peekLast();}else{filteredItems.push(i);}}
if(i<this._delegate.itemCount()){this._scoringTimer=setTimeout(scoreItems.bind(this,i),0);return;}
delete this._scoringTimer;this._filteredItems=bestItems.concat(overflowItems).concat(filteredItems);for(var i=0;i<this._filteredItems.length;++i){if(this._filteredItems[i]===oldSelectedAbsoluteIndex){this._selectedIndexInFiltered=i;break;}}
this._elements=[];this._viewportControl.refresh();if(!query)
this._selectedIndexInFiltered=0;this._updateSelection(this._selectedIndexInFiltered,false);this._itemsFilteredForTest();}}
_shouldShowMatchingItems(){return this._delegate.shouldShowMatchingItems(this._value());}
_onInput(){this._updateShowMatchingItems();this._scheduleFilter();}
_updateShowMatchingItems(){var shouldShowMatchingItems=this._shouldShowMatchingItems();this._itemElementsContainer.classList.toggle('hidden',!shouldShowMatchingItems);}
_rowsPerViewport(){return Math.floor(this._viewportControl.element.clientHeight/this._rowHeight);}
_onKeyDown(event){var newSelectedIndex=this._selectedIndexInFiltered;switch(event.keyCode){case UI.KeyboardShortcut.Keys.Down.code:if(++newSelectedIndex>=this._filteredItems.length)
newSelectedIndex=0;this._updateSelection(newSelectedIndex,true);event.consume(true);break;case UI.KeyboardShortcut.Keys.Up.code:if(--newSelectedIndex<0)
newSelectedIndex=this._filteredItems.length-1;this._updateSelection(newSelectedIndex,false);event.consume(true);break;case UI.KeyboardShortcut.Keys.PageDown.code:newSelectedIndex=Math.min(newSelectedIndex+this._rowsPerViewport(),this._filteredItems.length-1);this._updateSelection(newSelectedIndex,true);event.consume(true);break;case UI.KeyboardShortcut.Keys.PageUp.code:newSelectedIndex=Math.max(newSelectedIndex-this._rowsPerViewport(),0);this._updateSelection(newSelectedIndex,false);event.consume(true);break;case UI.KeyboardShortcut.Keys.Enter.code:this._onEnter(event);break;case UI.KeyboardShortcut.Keys.Tab.code:this._tabKeyPressed();break;default:}}
_scheduleFilter(){if(this._filterTimer)
return;this._filterTimer=setTimeout(this._filterItems.bind(this),0);}
_updateSelection(index,makeLast){if(!this._filteredItems.length)
return;if(this._selectedElement)
this._selectedElement.classList.remove('selected');this._viewportControl.scrollItemIntoView(index,makeLast);this._selectedIndexInFiltered=index;this._selectedElement=this._elements[index];if(this._selectedElement)
this._selectedElement.classList.add('selected');}
_onClick(event){var itemElement=event.target.enclosingNodeOrSelfWithClass('filtered-list-widget-item');if(!itemElement)
return;if(this._dialog)
this._dialog.detach();this._delegate.selectItemWithQuery(itemElement._index,this._value());}
itemCount(){return this._filteredItems.length;}
fastItemHeight(index){if(!this._rowHeight){var delegateIndex=this._filteredItems[index];var element=this._createItemElement(delegateIndex);this._rowHeight=UI.measurePreferredSize(element,this._itemElementsContainer).height;}
return this._rowHeight;}
itemElement(index){if(!this._elements[index])
this._elements[index]=this._createItemElement(this._filteredItems[index]);return this._elements[index];}};UI.FilteredListWidget.Delegate=class{constructor(promptHistory){this._promptHistory=promptHistory;}
setRefreshCallback(refreshCallback){this._refreshCallback=refreshCallback;}
shouldShowMatchingItems(query){return true;}
itemCount(){return 0;}
itemKeyAt(itemIndex){return'';}
itemScoreAt(itemIndex,query){return 1;}
renderItem(itemIndex,query,titleElement,subtitleElement){}
highlightRanges(element,query){if(!query)
return false;function rangesForMatch(text,query){var opcodes=Diff.Diff.charDiff(query,text);var offset=0;var ranges=[];for(var i=0;i<opcodes.length;++i){var opcode=opcodes[i];if(opcode[0]===Diff.Diff.Operation.Equal)
ranges.push(new Common.SourceRange(offset,opcode[1].length));else if(opcode[0]!==Diff.Diff.Operation.Insert)
return null;offset+=opcode[1].length;}
return ranges;}
var text=element.textContent;var ranges=rangesForMatch(text,query);if(!ranges||!this.caseSensitive())
ranges=rangesForMatch(text.toUpperCase(),query.toUpperCase());if(ranges){UI.highlightRangesWithStyleClass(element,ranges,'highlight');return true;}
return false;}
caseSensitive(){return true;}
renderMonospace(){return true;}
renderAsTwoRows(){return false;}
selectItemWithQuery(itemIndex,promptValue){this._promptHistory.push(promptValue);if(this._promptHistory.length>100)
this._promptHistory.shift();this.selectItem(itemIndex,promptValue);}
selectItem(itemIndex,promptValue){}
refresh(){this._refreshCallback();}
rewriteQuery(query){return query;}
autocomplete(query){for(var i=this._promptHistory.length-1;i>=0;i--){if(this._promptHistory[i]!==query&&this._promptHistory[i].startsWith(query))
return this._promptHistory[i];}
return query;}
dispose(){}};;UI.CommandMenu=class{constructor(){this._commands=[];this._loadCommands();}
static createCommand(category,keys,title,shortcut,executeHandler,availableHandler){var key=keys.replace(/,/g,'\0');return new UI.CommandMenu.Command(category,title,key,shortcut,executeHandler,availableHandler);}
static createSettingCommand(extension,title,value){var category=extension.descriptor()['category']||'';var tags=extension.descriptor()['tags']||'';var setting=Common.settings.moduleSetting(extension.descriptor()['settingName']);return UI.CommandMenu.createCommand(category,tags,title,'',setting.set.bind(setting,value),availableHandler);function availableHandler(){return setting.get()!==value;}}
static createActionCommand(action){var shortcut=UI.shortcutRegistry.shortcutTitleForAction(action.id())||'';return UI.CommandMenu.createCommand(action.category(),action.tags(),action.title(),shortcut,action.execute.bind(action));}
static createRevealPanelCommand(extension){var panelName=extension.descriptor()['name'];var tags=extension.descriptor()['tags']||'';return UI.CommandMenu.createCommand(Common.UIString('Panel'),tags,Common.UIString('Show %s',extension.title()),'',executeHandler,availableHandler);function availableHandler(){return true;}
function executeHandler(){UI.viewManager.showView(panelName);}}
static createRevealDrawerCommand(extension){var drawerId=extension.descriptor()['id'];var executeHandler=UI.viewManager.showView.bind(UI.viewManager,drawerId);var tags=extension.descriptor()['tags']||'';return UI.CommandMenu.createCommand(Common.UIString('Drawer'),tags,Common.UIString('Show %s',extension.title()),'',executeHandler);}
_loadCommands(){var panelExtensions=self.runtime.extensions(UI.Panel);for(var extension of panelExtensions)
this._commands.push(UI.CommandMenu.createRevealPanelCommand(extension));var drawerExtensions=self.runtime.extensions('view');for(var extension of drawerExtensions){if(extension.descriptor()['location']!=='drawer-view')
continue;this._commands.push(UI.CommandMenu.createRevealDrawerCommand(extension));}
var settingExtensions=self.runtime.extensions('setting');for(var extension of settingExtensions){var options=extension.descriptor()['options'];if(!options||!extension.descriptor()['category'])
continue;for(var pair of options)
this._commands.push(UI.CommandMenu.createSettingCommand(extension,pair['title'],pair['value']));}}
commands(){return this._commands;}};UI.CommandMenuDelegate=class extends UI.FilteredListWidget.Delegate{constructor(){super([]);this._commands=[];this._appendAvailableCommands();}
_appendAvailableCommands(){var allCommands=UI.commandMenu.commands();var actions=UI.actionRegistry.availableActions();for(var action of actions){if(action.category())
this._commands.push(UI.CommandMenu.createActionCommand(action));}
for(var command of allCommands){if(command.available())
this._commands.push(command);}
this._commands=this._commands.sort(commandComparator);function commandComparator(left,right){var cats=left.category().compareTo(right.category());return cats?cats:left.title().compareTo(right.title());}}
itemCount(){return this._commands.length;}
itemKeyAt(itemIndex){return this._commands[itemIndex].key();}
itemScoreAt(itemIndex,query){var command=this._commands[itemIndex];var opcodes=Diff.Diff.charDiff(query.toLowerCase(),command.title().toLowerCase());var score=0;for(var i=0;i<opcodes.length;++i){if(opcodes[i][0]===Diff.Diff.Operation.Equal)
score+=opcodes[i][1].length*opcodes[i][1].length;}
if(command.category().startsWith('Panel'))
score+=2;else if(command.category().startsWith('Drawer'))
score+=1;return score;}
renderItem(itemIndex,query,titleElement,subtitleElement){var command=this._commands[itemIndex];titleElement.removeChildren();var tagElement=titleElement.createChild('span','tag');var index=String.hashCode(command.category())%UI.CommandMenuDelegate.MaterialPaletteColors.length;tagElement.style.backgroundColor=UI.CommandMenuDelegate.MaterialPaletteColors[index];tagElement.textContent=command.category();titleElement.createTextChild(command.title());this.highlightRanges(titleElement,query);subtitleElement.textContent=command.shortcut();}
selectItem(itemIndex,promptValue){this._commands[itemIndex].execute();}
caseSensitive(){return false;}
renderMonospace(){return false;}};UI.CommandMenuDelegate.MaterialPaletteColors=['#F44336','#E91E63','#9C27B0','#673AB7','#3F51B5','#03A9F4','#00BCD4','#009688','#4CAF50','#8BC34A','#CDDC39','#FFC107','#FF9800','#FF5722','#795548','#9E9E9E','#607D8B'];UI.CommandMenu.Command=class{constructor(category,title,key,shortcut,executeHandler,availableHandler){this._category=category;this._title=title;this._key=category+'\0'+title+'\0'+key;this._shortcut=shortcut;this._executeHandler=executeHandler;this._availableHandler=availableHandler;}
category(){return this._category;}
title(){return this._title;}
key(){return this._key;}
shortcut(){return this._shortcut;}
available(){return this._availableHandler?this._availableHandler():true;}
execute(){this._executeHandler();}};UI.commandMenu=new UI.CommandMenu();UI.CommandMenu.ShowActionDelegate=class{handleAction(context,actionId){new UI.FilteredListWidget(new UI.CommandMenuDelegate()).showAsDialog();InspectorFrontendHost.bringToFront();return true;}};;UI.FlameChartDelegate=function(){};UI.FlameChartDelegate.prototype={requestWindowTimes:function(startTime,endTime){},updateRangeSelection:function(startTime,endTime){},};UI.FlameChart=class extends UI.ChartViewport{constructor(dataProvider,flameChartDelegate,groupExpansionSetting){super();this.registerRequiredCSS('ui_lazy/flameChart.css');this.contentElement.classList.add('flame-chart-main-pane');this._flameChartDelegate=flameChartDelegate;this._groupExpansionSetting=groupExpansionSetting;this._groupExpansionState=groupExpansionSetting&&groupExpansionSetting.get()||{};this._dataProvider=dataProvider;this._calculator=new UI.FlameChart.Calculator(dataProvider);this._canvas=(this.viewportElement.createChild('canvas'));this._canvas.tabIndex=1;this.setDefaultFocusedElement(this._canvas);this._canvas.addEventListener('mousemove',this._onMouseMove.bind(this),false);this._canvas.addEventListener('mouseout',this._onMouseOut.bind(this),false);this._canvas.addEventListener('click',this._onClick.bind(this),false);this._canvas.addEventListener('keydown',this._onKeyDown.bind(this),false);this._entryInfo=this.viewportElement.createChild('div','flame-chart-entry-info');this._markerHighlighElement=this.viewportElement.createChild('div','flame-chart-marker-highlight-element');this._highlightElement=this.viewportElement.createChild('div','flame-chart-highlight-element');this._selectedElement=this.viewportElement.createChild('div','flame-chart-selected-element');this._windowLeft=0.0;this._windowRight=1.0;this._timeWindowLeft=0;this._timeWindowRight=Infinity;this._rangeSelectionStart=0;this._rangeSelectionEnd=0;this._barHeight=dataProvider.barHeight();this._paddingLeft=this._dataProvider.paddingLeft();var markerPadding=2;this._markerRadius=this._barHeight/2-markerPadding;this._headerLeftPadding=6;this._arrowSide=8;this._expansionArrowIndent=this._headerLeftPadding+this._arrowSide/2;this._headerLabelXPadding=3;this._headerLabelYPadding=2;this._highlightedMarkerIndex=-1;this._highlightedEntryIndex=-1;this._selectedEntryIndex=-1;this._rawTimelineDataLength=0;this._textWidth=new Map();this._lastMouseOffsetX=0;}
willHide(){this.hideHighlight();}
highlightEntry(entryIndex){if(this._highlightedEntryIndex===entryIndex)
return;this._highlightedEntryIndex=entryIndex;this._updateElementPosition(this._highlightElement,this._highlightedEntryIndex);}
hideHighlight(){this._entryInfo.removeChildren();this._highlightedEntryIndex=-1;this._updateElementPosition(this._highlightElement,this._highlightedEntryIndex);}
_resetCanvas(){var ratio=window.devicePixelRatio;this._canvas.width=this._offsetWidth*ratio;this._canvas.height=this._offsetHeight*ratio;this._canvas.style.width=this._offsetWidth+'px';this._canvas.style.height=this._offsetHeight+'px';}
_timelineData(){if(!this._dataProvider)
return null;var timelineData=this._dataProvider.timelineData();if(timelineData!==this._rawTimelineData||timelineData.entryStartTimes.length!==this._rawTimelineDataLength)
this._processTimelineData(timelineData);return this._rawTimelineData;}
_revealEntry(entryIndex){var timelineData=this._timelineData();if(!timelineData)
return;var timeLeft=this._cancelWindowTimesAnimation?this._pendingAnimationTimeLeft:this._timeWindowLeft;var timeRight=this._cancelWindowTimesAnimation?this._pendingAnimationTimeRight:this._timeWindowRight;var entryStartTime=timelineData.entryStartTimes[entryIndex];var entryTotalTime=timelineData.entryTotalTimes[entryIndex];var entryEndTime=entryStartTime+entryTotalTime;var minEntryTimeWindow=Math.min(entryTotalTime,timeRight-timeLeft);var y=this._levelToHeight(timelineData.entryLevels[entryIndex]);this.setScrollOffset(y,this._barHeight);if(timeLeft>entryEndTime){var delta=timeLeft-entryEndTime+minEntryTimeWindow;this._flameChartDelegate.requestWindowTimes(timeLeft-delta,timeRight-delta);}else if(timeRight<entryStartTime){var delta=entryStartTime-timeRight+minEntryTimeWindow;this._flameChartDelegate.requestWindowTimes(timeLeft+delta,timeRight+delta);}}
setWindowTimes(startTime,endTime){super.setWindowTimes(startTime,endTime);this._updateHighlight();}
_onMouseMove(event){this._lastMouseOffsetX=event.offsetX;this._lastMouseOffsetY=event.offsetY;if(!this._enabled())
return;if(this.isDragging())
return;if(this._coordinatesToGroupIndex(event.offsetX,event.offsetY)>=0){this.hideHighlight();this.viewportElement.style.cursor='pointer';return;}
this._updateHighlight();}
_updateHighlight(){var inDividersBar=this._lastMouseOffsetY<UI.FlameChart.DividersBarHeight;this._highlightedMarkerIndex=inDividersBar?this._markerIndexAtPosition(this._lastMouseOffsetX):-1;this._updateMarkerHighlight();var entryIndex=this._coordinatesToEntryIndex(this._lastMouseOffsetX,this._lastMouseOffsetY);if(entryIndex===-1){this.hideHighlight();return;}
if(this.isDragging())
return;this._updatePopover(entryIndex);this.viewportElement.style.cursor=this._dataProvider.canJumpToEntry(entryIndex)?'pointer':'default';this.highlightEntry(entryIndex);}
_onMouseOut(){this._lastMouseOffsetX=-1;this._lastMouseOffsetY=-1;this.hideHighlight();}
_updatePopover(entryIndex){if(entryIndex===this._highlightedEntryIndex){this._updatePopoverOffset();return;}
this._entryInfo.removeChildren();var popoverElement=this._dataProvider.prepareHighlightedEntryInfo(entryIndex);if(popoverElement){this._entryInfo.appendChild(popoverElement);this._updatePopoverOffset();}}
_updatePopoverOffset(){var mouseX=this._lastMouseOffsetX;var mouseY=this._lastMouseOffsetY;var parentWidth=this._entryInfo.parentElement.clientWidth;var parentHeight=this._entryInfo.parentElement.clientHeight;var infoWidth=this._entryInfo.clientWidth;var infoHeight=this._entryInfo.clientHeight;var offsetX=10;var offsetY=6;var x;var y;for(var quadrant=0;quadrant<4;++quadrant){var dx=quadrant&2?-offsetX-infoWidth:offsetX;var dy=quadrant&1?-offsetY-infoHeight:offsetY;x=Number.constrain(mouseX+dx,0,parentWidth-infoWidth);y=Number.constrain(mouseY+dy,0,parentHeight-infoHeight);if(x>=mouseX||mouseX>=x+infoWidth||y>=mouseY||mouseY>=y+infoHeight)
break;}
this._entryInfo.style.left=x+'px';this._entryInfo.style.top=y+'px';}
_onClick(event){this.focus();const clickThreshold=5;if(this.maxDragOffset()>clickThreshold)
return;var groupIndex=this._coordinatesToGroupIndex(event.offsetX,event.offsetY);if(groupIndex>=0){this._toggleGroupVisibility(groupIndex);return;}
this.hideRangeSelection();this.dispatchEventToListeners(UI.FlameChart.Events.EntrySelected,this._highlightedEntryIndex);}
_toggleGroupVisibility(groupIndex){if(!this._isGroupCollapsible(groupIndex))
return;var groups=this._rawTimelineData.groups;var group=groups[groupIndex];group.expanded=!group.expanded;this._groupExpansionState[group.name]=group.expanded;if(this._groupExpansionSetting)
this._groupExpansionSetting.set(this._groupExpansionState);this._updateLevelPositions();this._updateHighlight();if(!group.expanded){var timelineData=this._timelineData();var level=timelineData.entryLevels[this._selectedEntryIndex];if(this._selectedEntryIndex>=0&&level>=group.startLevel&&(groupIndex===groups.length||groups[groupIndex+1].startLevel>level))
this._selectedEntryIndex=-1;}
this._updateHeight();this._resetCanvas();this._draw(this._offsetWidth,this._offsetHeight);}
_onKeyDown(e){this._handleSelectionNavigation(e);}
_handleSelectionNavigation(e){if(!UI.KeyboardShortcut.hasNoModifiers(e))
return;if(this._selectedEntryIndex===-1)
return;var timelineData=this._timelineData();if(!timelineData)
return;function timeComparator(time,entryIndex){return time-timelineData.entryStartTimes[entryIndex];}
function entriesIntersect(entry1,entry2){var start1=timelineData.entryStartTimes[entry1];var start2=timelineData.entryStartTimes[entry2];var end1=start1+timelineData.entryTotalTimes[entry1];var end2=start2+timelineData.entryTotalTimes[entry2];return start1<end2&&start2<end1;}
var keys=UI.KeyboardShortcut.Keys;if(e.keyCode===keys.Left.code||e.keyCode===keys.Right.code){var level=timelineData.entryLevels[this._selectedEntryIndex];var levelIndexes=this._timelineLevels[level];var indexOnLevel=levelIndexes.lowerBound(this._selectedEntryIndex);indexOnLevel+=e.keyCode===keys.Left.code?-1:1;e.consume(true);if(indexOnLevel>=0&&indexOnLevel<levelIndexes.length)
this.dispatchEventToListeners(UI.FlameChart.Events.EntrySelected,levelIndexes[indexOnLevel]);return;}
if(e.keyCode===keys.Up.code||e.keyCode===keys.Down.code){e.consume(true);var level=timelineData.entryLevels[this._selectedEntryIndex];level+=e.keyCode===keys.Up.code?-1:1;if(level<0||level>=this._timelineLevels.length)
return;var entryTime=timelineData.entryStartTimes[this._selectedEntryIndex]+
timelineData.entryTotalTimes[this._selectedEntryIndex]/2;var levelIndexes=this._timelineLevels[level];var indexOnLevel=levelIndexes.upperBound(entryTime,timeComparator)-1;if(!entriesIntersect(this._selectedEntryIndex,levelIndexes[indexOnLevel])){++indexOnLevel;if(indexOnLevel>=levelIndexes.length||!entriesIntersect(this._selectedEntryIndex,levelIndexes[indexOnLevel]))
return;}
this.dispatchEventToListeners(UI.FlameChart.Events.EntrySelected,levelIndexes[indexOnLevel]);}}
_cursorTime(x){return(x+this._pixelWindowLeft-this._paddingLeft)*this._pixelToTime+this._minimumBoundary;}
_coordinatesToEntryIndex(x,y){if(x<0||y<0)
return-1;y+=this.scrollOffset();var timelineData=this._timelineData();if(!timelineData)
return-1;var cursorTime=this._cursorTime(x);var cursorLevel=this._visibleLevelOffsets.upperBound(y)-1;if(cursorLevel<0||!this._visibleLevels[cursorLevel])
return-1;var offsetFromLevel=y-this._visibleLevelOffsets[cursorLevel];if(offsetFromLevel>this._barHeight)
return-1;var entryStartTimes=timelineData.entryStartTimes;var entryTotalTimes=timelineData.entryTotalTimes;var entryIndexes=this._timelineLevels[cursorLevel];if(!entryIndexes||!entryIndexes.length)
return-1;function comparator(time,entryIndex){return time-entryStartTimes[entryIndex];}
var indexOnLevel=Math.max(entryIndexes.upperBound(cursorTime,comparator)-1,0);function checkEntryHit(entryIndex){if(entryIndex===undefined)
return false;var startTime=entryStartTimes[entryIndex];var duration=entryTotalTimes[entryIndex];if(isNaN(duration)){var dx=(startTime-cursorTime)/this._pixelToTime;var dy=this._barHeight/2-offsetFromLevel;return dx*dx+dy*dy<this._markerRadius*this._markerRadius;}
var endTime=startTime+duration;var barThreshold=3*this._pixelToTime;return startTime-barThreshold<cursorTime&&cursorTime<endTime+barThreshold;}
var entryIndex=entryIndexes[indexOnLevel];if(checkEntryHit.call(this,entryIndex))
return entryIndex;entryIndex=entryIndexes[indexOnLevel+1];if(checkEntryHit.call(this,entryIndex))
return entryIndex;return-1;}
_coordinatesToGroupIndex(x,y){if(x<0||y<0)
return-1;y+=this.scrollOffset();var groups=this._rawTimelineData.groups||[];var group=this._groupOffsets.upperBound(y)-1;if(group<0||group>=groups.length||y-this._groupOffsets[group]>=groups[group].style.height)
return-1;var context=(this._canvas.getContext('2d'));context.save();context.font=groups[group].style.font;var right=this._headerLeftPadding+this._labelWidthForGroup(context,groups[group]);context.restore();if(x>right)
return-1;return group;}
_markerIndexAtPosition(x){var markers=this._timelineData().markers;if(!markers)
return-1;var accurracyOffsetPx=1;var time=this._cursorTime(x);var leftTime=this._cursorTime(x-accurracyOffsetPx);var rightTime=this._cursorTime(x+accurracyOffsetPx);var left=this._markerIndexBeforeTime(leftTime);var markerIndex=-1;var distance=Infinity;for(var i=left;i<markers.length&&markers[i].startTime()<rightTime;i++){var nextDistance=Math.abs(markers[i].startTime()-time);if(nextDistance<distance){markerIndex=i;distance=nextDistance;}}
return markerIndex;}
_markerIndexBeforeTime(time){return this._timelineData().markers.lowerBound(time,(markerTimestamp,marker)=>markerTimestamp-marker.startTime());}
_draw(width,height){var timelineData=this._timelineData();if(!timelineData)
return;var context=(this._canvas.getContext('2d'));context.save();var ratio=window.devicePixelRatio;var top=this.scrollOffset();context.scale(ratio,ratio);context.translate(0,-top);var defaultFont='11px '+Host.fontFamily();context.font=defaultFont;var timeWindowRight=this._timeWindowRight;var timeWindowLeft=this._timeWindowLeft-this._paddingLeft/this._timeToPixel;var entryTotalTimes=timelineData.entryTotalTimes;var entryStartTimes=timelineData.entryStartTimes;var entryLevels=timelineData.entryLevels;var titleIndices=[];var markerIndices=[];var textPadding=this._dataProvider.textPadding();var minTextWidth=2*textPadding+UI.measureTextWidth(context,'\u2026');var barHeight=this._barHeight;var minVisibleBarLevel=Math.max(this._visibleLevelOffsets.upperBound(top)-1,0);var colorBuckets=new Map();for(var level=minVisibleBarLevel;level<this._dataProvider.maxStackDepth();++level){if(this._levelToHeight(level)>top+height)
break;if(!this._visibleLevels[level])
continue;var levelIndexes=this._timelineLevels[level];var rightIndexOnLevel=levelIndexes.lowerBound(timeWindowRight,(time,entryIndex)=>time-entryStartTimes[entryIndex])-1;var lastDrawOffset=Infinity;for(var entryIndexOnLevel=rightIndexOnLevel;entryIndexOnLevel>=0;--entryIndexOnLevel){var entryIndex=levelIndexes[entryIndexOnLevel];var entryStartTime=entryStartTimes[entryIndex];var entryOffsetRight=entryStartTime+(entryTotalTimes[entryIndex]||0);if(entryOffsetRight<=timeWindowLeft)
break;var barX=this._timeToPositionClipped(entryStartTime);if(barX>=lastDrawOffset)
continue;lastDrawOffset=barX;var color=this._dataProvider.entryColor(entryIndex);var bucket=colorBuckets.get(color);if(!bucket){bucket=[];colorBuckets.set(color,bucket);}
bucket.push(entryIndex);}}
var colors=colorBuckets.keysArray();for(var c=0;c<colors.length;++c){var color=colors[c];var indexes=colorBuckets.get(color);context.beginPath();context.fillStyle=color;for(var i=0;i<indexes.length;++i){var entryIndex=indexes[i];var entryStartTime=entryStartTimes[entryIndex];var barX=this._timeToPositionClipped(entryStartTime);var duration=entryTotalTimes[entryIndex];var barLevel=entryLevels[entryIndex];var barY=this._levelToHeight(barLevel);if(isNaN(duration)){context.moveTo(barX+this._markerRadius,barY+barHeight/2);context.arc(barX,barY+barHeight/2,this._markerRadius,0,Math.PI*2);markerIndices.push(entryIndex);continue;}
var barRight=this._timeToPositionClipped(entryStartTime+duration);var barWidth=Math.max(barRight-barX,1);context.rect(barX,barY,barWidth-0.4,barHeight-1);if(barWidth>minTextWidth||this._dataProvider.forceDecoration(entryIndex))
titleIndices.push(entryIndex);}
context.fill();}
context.strokeStyle='rgba(0, 0, 0, 0.2)';context.beginPath();for(var m=0;m<markerIndices.length;++m){var entryIndex=markerIndices[m];var entryStartTime=entryStartTimes[entryIndex];var barX=this._timeToPositionClipped(entryStartTime);var barLevel=entryLevels[entryIndex];var barY=this._levelToHeight(barLevel);context.moveTo(barX+this._markerRadius,barY+barHeight/2);context.arc(barX,barY+barHeight/2,this._markerRadius,0,Math.PI*2);}
context.stroke();context.textBaseline='alphabetic';var textBaseHeight=this._barHeight-this._dataProvider.textBaseline();for(var i=0;i<titleIndices.length;++i){var entryIndex=titleIndices[i];var entryStartTime=entryStartTimes[entryIndex];var barX=this._timeToPositionClipped(entryStartTime);var barRight=Math.min(this._timeToPositionClipped(entryStartTime+entryTotalTimes[entryIndex]),width)+1;var barWidth=barRight-barX;var barLevel=entryLevels[entryIndex];var barY=this._levelToHeight(barLevel);var text=this._dataProvider.entryTitle(entryIndex);if(text&&text.length){context.font=this._dataProvider.entryFont(entryIndex)||defaultFont;text=UI.trimTextMiddle(context,text,barWidth-2*textPadding);}
var unclippedBarX=this._timeToPosition(entryStartTime);if(this._dataProvider.decorateEntry(entryIndex,context,text,barX,barY,barWidth,barHeight,unclippedBarX,this._timeToPixel))
continue;if(!text||!text.length)
continue;context.fillStyle=this._dataProvider.textColor(entryIndex);context.fillText(text,barX+textPadding,barY+textBaseHeight);}
this._drawFlowEvents(context,width,height);context.restore();UI.TimelineGrid.drawCanvasGrid(context,this._calculator,3);this._drawMarkers();this._drawGroupHeaders(width,height);this._updateElementPosition(this._highlightElement,this._highlightedEntryIndex);this._updateElementPosition(this._selectedElement,this._selectedEntryIndex);this._updateMarkerHighlight();}
_drawGroupHeaders(width,height){var context=(this._canvas.getContext('2d'));var top=this.scrollOffset();var ratio=window.devicePixelRatio;var barHeight=this._barHeight;var textBaseHeight=barHeight-this._dataProvider.textBaseline();var groups=this._rawTimelineData.groups||[];if(!groups.length)
return;var groupOffsets=this._groupOffsets;var lastGroupOffset=Array.prototype.peekLast.call(groupOffsets);var colorUsage=UI.ThemeSupport.ColorUsage;context.save();context.scale(ratio,ratio);context.translate(0,-top);context.fillStyle=UI.themeSupport.patchColor('#eee',colorUsage.Background);forEachGroup.call(this,(offset,index,group)=>{var paddingHeight=group.style.padding;if(paddingHeight<5)
return;context.fillRect(0,offset-paddingHeight+2,width,paddingHeight-4);});if(groups.length&&lastGroupOffset<top+height)
context.fillRect(0,lastGroupOffset+2,width,top+height-lastGroupOffset);context.strokeStyle=UI.themeSupport.patchColor('#bbb',colorUsage.Background);context.beginPath();forEachGroup.call(this,(offset,index,group,isFirst)=>{if(isFirst||group.style.padding<4)
return;hLine(offset-2.5);});hLine(lastGroupOffset+0.5);context.stroke();forEachGroup.call(this,(offset,index,group)=>{if(group.style.useFirstLineForOverview)
return;if(!this._isGroupCollapsible(index)||group.expanded){if(!group.style.shareHeaderLine){context.fillStyle=group.style.backgroundColor;context.fillRect(0,offset,width,group.style.height);}
return;}
var nextGroup=index+1;while(nextGroup<groups.length&&groups[nextGroup].style.nestingLevel>group.style.nestingLevel)
nextGroup++;var endLevel=nextGroup<groups.length?groups[nextGroup].startLevel:this._dataProvider.maxStackDepth();this._drawCollapsedOverviewForGroup(offset+1,group.startLevel,endLevel);});context.save();forEachGroup.call(this,(offset,index,group)=>{context.font=group.style.font;if(this._isGroupCollapsible(index)&&!group.expanded||group.style.shareHeaderLine){var width=this._labelWidthForGroup(context,group);context.fillStyle=Common.Color.parse(group.style.backgroundColor).setAlpha(0.7).asString(null);context.fillRect(this._headerLeftPadding-this._headerLabelXPadding,offset+this._headerLabelYPadding,width,barHeight-2*this._headerLabelYPadding);}
context.fillStyle=group.style.color;context.fillText(group.name,Math.floor(this._expansionArrowIndent*(group.style.nestingLevel+1)+this._arrowSide),offset+textBaseHeight);});context.restore();context.fillStyle=UI.themeSupport.patchColor('#6e6e6e',colorUsage.Foreground);context.beginPath();forEachGroup.call(this,(offset,index,group)=>{if(this._isGroupCollapsible(index)){drawExpansionArrow.call(this,this._expansionArrowIndent*(group.style.nestingLevel+1),offset+textBaseHeight-this._arrowSide/2,!!group.expanded);}});context.fill();context.strokeStyle=UI.themeSupport.patchColor('#ddd',colorUsage.Background);context.beginPath();context.stroke();context.restore();function hLine(y){context.moveTo(0,y);context.lineTo(width,y);}
function drawExpansionArrow(x,y,expanded){var arrowHeight=this._arrowSide*Math.sqrt(3)/2;var arrowCenterOffset=Math.round(arrowHeight/2);context.save();context.translate(x,y);context.rotate(expanded?Math.PI/2:0);context.moveTo(-arrowCenterOffset,-this._arrowSide/2);context.lineTo(-arrowCenterOffset,this._arrowSide/2);context.lineTo(arrowHeight-arrowCenterOffset,0);context.restore();}
function forEachGroup(callback){var groupStack=[{nestingLevel:-1,visible:true}];for(var i=0;i<groups.length;++i){var groupTop=groupOffsets[i];var group=groups[i];if(groupTop-group.style.padding>top+height)
break;var firstGroup=true;while(groupStack.peekLast().nestingLevel>=group.style.nestingLevel){groupStack.pop();firstGroup=false;}
var parentGroupVisible=groupStack.peekLast().visible;var thisGroupVisible=parentGroupVisible&&(!this._isGroupCollapsible(i)||group.expanded);groupStack.push({nestingLevel:group.style.nestingLevel,visible:thisGroupVisible});if(!parentGroupVisible||groupTop+group.style.height<top)
continue;callback(groupTop,i,group,firstGroup);}}}
_labelWidthForGroup(context,group){return UI.measureTextWidth(context,group.name)+this._expansionArrowIndent*(group.style.nestingLevel+1)+
2*this._headerLabelXPadding;}
_drawCollapsedOverviewForGroup(y,startLevel,endLevel){var range=new Common.SegmentedRange(mergeCallback);var timeWindowRight=this._timeWindowRight;var timeWindowLeft=this._timeWindowLeft-this._paddingLeft/this._timeToPixel;var context=(this._canvas.getContext('2d'));var barHeight=this._barHeight-2;var entryStartTimes=this._rawTimelineData.entryStartTimes;var entryTotalTimes=this._rawTimelineData.entryTotalTimes;for(var level=startLevel;level<endLevel;++level){var levelIndexes=this._timelineLevels[level];var rightIndexOnLevel=levelIndexes.lowerBound(timeWindowRight,(time,entryIndex)=>time-entryStartTimes[entryIndex])-1;var lastDrawOffset=Infinity;for(var entryIndexOnLevel=rightIndexOnLevel;entryIndexOnLevel>=0;--entryIndexOnLevel){var entryIndex=levelIndexes[entryIndexOnLevel];var entryStartTime=entryStartTimes[entryIndex];var startPosition=this._timeToPositionClipped(entryStartTime);var entryEndTime=entryStartTime+entryTotalTimes[entryIndex];if(isNaN(entryEndTime)||startPosition>=lastDrawOffset)
continue;if(entryEndTime<=timeWindowLeft)
break;lastDrawOffset=startPosition;var color=this._dataProvider.entryColor(entryIndex);range.append(new Common.Segment(startPosition,this._timeToPositionClipped(entryEndTime),color));}}
var segments=range.segments().slice().sort((a,b)=>a.data.localeCompare(b.data));var lastColor;context.beginPath();for(var i=0;i<segments.length;++i){var segment=segments[i];if(lastColor!==segments[i].data){context.fill();context.beginPath();lastColor=segments[i].data;context.fillStyle=lastColor;}
context.rect(segment.begin,y,segment.end-segment.begin,barHeight);}
context.fill();function mergeCallback(a,b){return a.data===b.data&&a.end+0.4>b.end?a:null;}}
_drawFlowEvents(context,width,height){var timelineData=this._timelineData();var timeWindowRight=this._timeWindowRight;var timeWindowLeft=this._timeWindowLeft;var flowStartTimes=timelineData.flowStartTimes;var flowEndTimes=timelineData.flowEndTimes;var flowStartLevels=timelineData.flowStartLevels;var flowEndLevels=timelineData.flowEndLevels;var flowCount=flowStartTimes.length;var endIndex=flowStartTimes.lowerBound(timeWindowRight);var color=[];var fadeColorsCount=8;for(var i=0;i<=fadeColorsCount;++i)
color[i]='rgba(128, 0, 0, '+i/fadeColorsCount+')';var fadeColorsRange=color.length;var minimumFlowDistancePx=15;var flowArcHeight=4*this._barHeight;var colorIndex=0;context.lineWidth=0.5;for(var i=0;i<endIndex;++i){if(flowEndTimes[i]<timeWindowLeft)
continue;var startX=this._timeToPosition(flowStartTimes[i]);var endX=this._timeToPosition(flowEndTimes[i]);if(endX-startX<minimumFlowDistancePx)
continue;if(startX<-minimumFlowDistancePx&&endX>width+minimumFlowDistancePx)
continue;if(endX-startX<minimumFlowDistancePx+fadeColorsRange||colorIndex!==color.length-1){colorIndex=Math.min(fadeColorsRange-1,Math.floor(endX-startX-minimumFlowDistancePx));context.strokeStyle=color[colorIndex];}
var startY=this._levelToHeight(flowStartLevels[i])+this._barHeight;var endY=this._levelToHeight(flowEndLevels[i]);context.beginPath();context.moveTo(startX,startY);var arcHeight=Math.max(Math.sqrt(Math.abs(startY-endY)),flowArcHeight)+5;context.bezierCurveTo(startX,startY+arcHeight,endX,endY+arcHeight,endX,endY+this._barHeight);context.stroke();}}
_drawMarkers(){var markers=this._timelineData().markers;var left=this._markerIndexBeforeTime(this._calculator.minimumBoundary());var rightBoundary=this._calculator.maximumBoundary();var context=(this._canvas.getContext('2d'));context.save();var ratio=window.devicePixelRatio;context.scale(ratio,ratio);var height=UI.FlameChart.DividersBarHeight-1;for(var i=left;i<markers.length;i++){var timestamp=markers[i].startTime();if(timestamp>rightBoundary)
break;markers[i].draw(context,this._calculator.computePosition(timestamp),height,this._timeToPixel);}
context.restore();}
_updateMarkerHighlight(){var element=this._markerHighlighElement;if(element.parentElement)
element.remove();var markerIndex=this._highlightedMarkerIndex;if(markerIndex===-1)
return;var marker=this._timelineData().markers[markerIndex];var barX=this._timeToPositionClipped(marker.startTime());element.title=marker.title();var style=element.style;style.left=barX+'px';style.backgroundColor=marker.color();this.viewportElement.appendChild(element);}
_processTimelineData(timelineData){if(!timelineData){this._timelineLevels=null;this._visibleLevelOffsets=null;this._visibleLevels=null;this._groupOffsets=null;this._rawTimelineData=null;this._rawTimelineDataLength=0;return;}
this._rawTimelineData=timelineData;this._rawTimelineDataLength=timelineData.entryStartTimes.length;var entryCounters=new Uint32Array(this._dataProvider.maxStackDepth()+1);for(var i=0;i<timelineData.entryLevels.length;++i)
++entryCounters[timelineData.entryLevels[i]];var levelIndexes=new Array(entryCounters.length);for(var i=0;i<levelIndexes.length;++i){levelIndexes[i]=new Uint32Array(entryCounters[i]);entryCounters[i]=0;}
for(var i=0;i<timelineData.entryLevels.length;++i){var level=timelineData.entryLevels[i];levelIndexes[level][entryCounters[level]++]=i;}
this._timelineLevels=levelIndexes;var groups=this._rawTimelineData.groups||[];for(var i=0;i<groups.length;++i){var expanded=this._groupExpansionState[groups[i].name];if(expanded!==undefined)
groups[i].expanded=expanded;}
this._updateLevelPositions();this._updateHeight();}
_updateLevelPositions(){var levelCount=this._dataProvider.maxStackDepth();var groups=this._rawTimelineData.groups||[];this._visibleLevelOffsets=new Uint32Array(levelCount+1);this._visibleLevels=new Uint16Array(levelCount);this._groupOffsets=new Uint32Array(groups.length+1);var groupIndex=-1;var currentOffset=UI.FlameChart.DividersBarHeight;var visible=true;var groupStack=[{nestingLevel:-1,visible:true}];for(var level=0;level<levelCount;++level){while(groupIndex<groups.length-1&&level===groups[groupIndex+1].startLevel){++groupIndex;var style=groups[groupIndex].style;var nextLevel=true;while(groupStack.peekLast().nestingLevel>=style.nestingLevel){groupStack.pop();nextLevel=false;}
var thisGroupIsVisible=groupIndex>=0&&this._isGroupCollapsible(groupIndex)?groups[groupIndex].expanded:true;var parentGroupIsVisible=groupStack.peekLast().visible;visible=thisGroupIsVisible&&parentGroupIsVisible;groupStack.push({nestingLevel:style.nestingLevel,visible:visible});if(parentGroupIsVisible)
currentOffset+=nextLevel?0:style.padding;this._groupOffsets[groupIndex]=currentOffset;if(parentGroupIsVisible&&!style.shareHeaderLine)
currentOffset+=style.height;}
var isFirstOnLevel=groupIndex>=0&&level===groups[groupIndex].startLevel;var thisLevelIsVisible=visible||isFirstOnLevel&&groups[groupIndex].style.useFirstLineForOverview;this._visibleLevels[level]=thisLevelIsVisible;this._visibleLevelOffsets[level]=currentOffset;if(thisLevelIsVisible||(parentGroupIsVisible&&style.shareHeaderLine&&isFirstOnLevel))
currentOffset+=this._barHeight;}
if(groupIndex>=0)
this._groupOffsets[groupIndex+1]=currentOffset;this._visibleLevelOffsets[level]=currentOffset;}
_isGroupCollapsible(index){var groups=this._rawTimelineData.groups||[];var style=groups[index].style;if(!style.shareHeaderLine||!style.collapsible)
return!!style.collapsible;var isLastGroup=index+1>=groups.length;if(!isLastGroup&&groups[index+1].style.nestingLevel>style.nestingLevel)
return true;var nextGroupLevel=isLastGroup?this._dataProvider.maxStackDepth():groups[index+1].startLevel;return nextGroupLevel!==groups[index].startLevel+1;}
setSelectedEntry(entryIndex){if(entryIndex===-1&&!this.isDragging())
this.hideRangeSelection();if(this._selectedEntryIndex===entryIndex)
return;this._selectedEntryIndex=entryIndex;this._revealEntry(entryIndex);this._updateElementPosition(this._selectedElement,this._selectedEntryIndex);}
_updateElementPosition(element,entryIndex){const elementMinWidthPx=2;if(element.parentElement)
element.remove();if(entryIndex===-1)
return;var timelineData=this._timelineData();var startTime=timelineData.entryStartTimes[entryIndex];var endTime=startTime+(timelineData.entryTotalTimes[entryIndex]||0);var barX=this._timeToPositionClipped(startTime);var barRight=this._timeToPositionClipped(endTime);if(barRight===0||barX===this._offsetWidth)
return;var barWidth=barRight-barX;var barCenter=barX+barWidth/2;barWidth=Math.max(barWidth,elementMinWidthPx);barX=barCenter-barWidth/2;var barY=this._levelToHeight(timelineData.entryLevels[entryIndex])-this.scrollOffset();var style=element.style;style.left=barX+'px';style.top=barY+'px';style.width=barWidth+'px';style.height=this._barHeight-1+'px';this.viewportElement.appendChild(element);}
_timeToPositionClipped(time){return Number.constrain(this._timeToPosition(time),0,this._offsetWidth);}
_timeToPosition(time){return Math.floor((time-this._minimumBoundary)*this._timeToPixel)-this._pixelWindowLeft+this._paddingLeft;}
_levelToHeight(level){return this._visibleLevelOffsets[level];}
_updateBoundaries(){this._totalTime=this._dataProvider.totalTime();this._minimumBoundary=this._dataProvider.minimumBoundary();var windowWidth=1;if(this._timeWindowRight!==Infinity){this._windowLeft=(this._timeWindowLeft-this._minimumBoundary)/this._totalTime;this._windowRight=(this._timeWindowRight-this._minimumBoundary)/this._totalTime;windowWidth=this._windowRight-this._windowLeft;}else if(this._timeWindowLeft===Infinity){this._windowLeft=Infinity;this._windowRight=Infinity;}else{this._windowLeft=0;this._windowRight=1;}
var totalPixels=Math.floor((this._offsetWidth-this._paddingLeft)/windowWidth);this._pixelWindowLeft=Math.floor(totalPixels*this._windowLeft);this._timeToPixel=totalPixels/this._totalTime;this._pixelToTime=this._totalTime/totalPixels;}
_updateHeight(){var height=this._levelToHeight(this._dataProvider.maxStackDepth());this.setContentHeight(height);}
onResize(){super.onResize();this.scheduleUpdate();}
update(){if(!this._timelineData())
return;this._resetCanvas();this._updateHeight();this._updateBoundaries();this._calculator._updateBoundaries(this);this._draw(this._offsetWidth,this._offsetHeight);if(!this.isDragging())
this._updateHighlight();}
reset(){super.reset();this._highlightedMarkerIndex=-1;this._highlightedEntryIndex=-1;this._selectedEntryIndex=-1;this._textWidth=new Map();this.update();}
_enabled(){return this._rawTimelineDataLength!==0;}};UI.FlameChart.DividersBarHeight=18;UI.FlameChart.MinimalTimeWindowMs=0.5;UI.FlameChartDataProvider=function(){};UI.FlameChart.Group;UI.FlameChart.GroupStyle;UI.FlameChart.TimelineData=class{constructor(entryLevels,entryTotalTimes,entryStartTimes,groups){this.entryLevels=entryLevels;this.entryTotalTimes=entryTotalTimes;this.entryStartTimes=entryStartTimes;this.groups=groups;this.markers=[];this.flowStartTimes=[];this.flowStartLevels=[];this.flowEndTimes=[];this.flowEndLevels=[];}};UI.FlameChartDataProvider.prototype={barHeight:function(){},minimumBoundary:function(){},totalTime:function(){},formatValue:function(value,precision){},maxStackDepth:function(){},timelineData:function(){},prepareHighlightedEntryInfo:function(entryIndex){},canJumpToEntry:function(entryIndex){},entryTitle:function(entryIndex){},entryFont:function(entryIndex){},entryColor:function(entryIndex){},decorateEntry:function(entryIndex,context,text,barX,barY,barWidth,barHeight,unclippedBarX,timeToPixels){},forceDecoration:function(entryIndex){},textColor:function(entryIndex){},textBaseline:function(){},textPadding:function(){},paddingLeft:function(){},};UI.FlameChartMarker=function(){};UI.FlameChartMarker.prototype={startTime:function(){},color:function(){},title:function(){},draw:function(context,x,height,pixelsPerMillisecond){},};UI.FlameChart.Events={EntrySelected:Symbol('EntrySelected')};UI.FlameChart.ColorGenerator=class{constructor(hueSpace,satSpace,lightnessSpace,alphaSpace){this._hueSpace=hueSpace||{min:0,max:360};this._satSpace=satSpace||67;this._lightnessSpace=lightnessSpace||80;this._alphaSpace=alphaSpace||1;this._colors=new Map();}
setColorForID(id,color){this._colors.set(id,color);}
colorForID(id){var color=this._colors.get(id);if(!color){color=this._generateColorForID(id);this._colors.set(id,color);}
return color;}
_generateColorForID(id){var hash=String.hashCode(id);var h=this._indexToValueInSpace(hash,this._hueSpace);var s=this._indexToValueInSpace(hash>>8,this._satSpace);var l=this._indexToValueInSpace(hash>>16,this._lightnessSpace);var a=this._indexToValueInSpace(hash>>24,this._alphaSpace);return'hsla('+h+', '+s+'%, '+l+'%, '+a+')';}
_indexToValueInSpace(index,space){if(typeof space==='number')
return space;var count=space.count||space.max-space.min;index%=count;return space.min+Math.floor(index/(count-1)*(space.max-space.min));}};UI.FlameChart.Calculator=class{constructor(dataProvider){this._dataProvider=dataProvider;this._paddingLeft=0;}
paddingLeft(){return this._paddingLeft;}
_updateBoundaries(mainPane){this._totalTime=mainPane._dataProvider.totalTime();this._zeroTime=mainPane._dataProvider.minimumBoundary();this._minimumBoundaries=this._zeroTime+mainPane._windowLeft*this._totalTime;this._maximumBoundaries=this._zeroTime+mainPane._windowRight*this._totalTime;this._paddingLeft=mainPane._paddingLeft;this._width=mainPane._offsetWidth-this._paddingLeft;this._timeToPixel=this._width/this.boundarySpan();}
computePosition(time){return Math.round((time-this._minimumBoundaries)*this._timeToPixel+this._paddingLeft);}
formatValue(value,precision){return this._dataProvider.formatValue(value-this._zeroTime,precision);}
maximumBoundary(){return this._maximumBoundaries;}
minimumBoundary(){return this._minimumBoundaries;}
zeroTime(){return this._zeroTime;}
boundarySpan(){return this._maximumBoundaries-this._minimumBoundaries;}};;UI.OverviewGrid=class{constructor(prefix){this.element=createElement('div');this.element.id=prefix+'-overview-container';this._grid=new UI.TimelineGrid();this._grid.element.id=prefix+'-overview-grid';this._grid.setScrollTop(0);this.element.appendChild(this._grid.element);this._window=new UI.OverviewGrid.Window(this.element,this._grid.dividersLabelBarElement);}
clientWidth(){return this.element.clientWidth;}
updateDividers(calculator){this._grid.updateDividers(calculator);}
addEventDividers(dividers){this._grid.addEventDividers(dividers);}
removeEventDividers(){this._grid.removeEventDividers();}
reset(){this._window.reset();}
windowLeft(){return this._window.windowLeft;}
windowRight(){return this._window.windowRight;}
setWindow(left,right){this._window._setWindow(left,right);}
addEventListener(eventType,listener,thisObject){return this._window.addEventListener(eventType,listener,thisObject);}
zoom(zoomFactor,referencePoint){this._window._zoom(zoomFactor,referencePoint);}
setResizeEnabled(enabled){this._window.setEnabled(enabled);}};UI.OverviewGrid.MinSelectableSize=14;UI.OverviewGrid.WindowScrollSpeedFactor=.3;UI.OverviewGrid.ResizerOffset=3.5;UI.OverviewGrid.Window=class extends Common.Object{constructor(parentElement,dividersLabelBarElement){super();this._parentElement=parentElement;UI.installDragHandle(this._parentElement,this._startWindowSelectorDragging.bind(this),this._windowSelectorDragging.bind(this),this._endWindowSelectorDragging.bind(this),'text',null);if(dividersLabelBarElement){UI.installDragHandle(dividersLabelBarElement,this._startWindowDragging.bind(this),this._windowDragging.bind(this),null,'-webkit-grabbing','-webkit-grab');}
this._parentElement.addEventListener('mousewheel',this._onMouseWheel.bind(this),true);this._parentElement.addEventListener('dblclick',this._resizeWindowMaximum.bind(this),true);UI.appendStyle(this._parentElement,'ui_lazy/overviewGrid.css');this._leftResizeElement=parentElement.createChild('div','overview-grid-window-resizer');UI.installDragHandle(this._leftResizeElement,this._resizerElementStartDragging.bind(this),this._leftResizeElementDragging.bind(this),null,'ew-resize');this._rightResizeElement=parentElement.createChild('div','overview-grid-window-resizer');UI.installDragHandle(this._rightResizeElement,this._resizerElementStartDragging.bind(this),this._rightResizeElementDragging.bind(this),null,'ew-resize');this._leftCurtainElement=parentElement.createChild('div','window-curtain-left');this._rightCurtainElement=parentElement.createChild('div','window-curtain-right');this.reset();}
reset(){this.windowLeft=0.0;this.windowRight=1.0;this.setEnabled(true);this._updateCurtains();}
setEnabled(enabled){this._enabled=enabled;}
_resizerElementStartDragging(event){if(!this._enabled)
return false;this._resizerParentOffsetLeft=event.pageX-event.offsetX-event.target.offsetLeft;event.stopPropagation();return true;}
_leftResizeElementDragging(event){this._resizeWindowLeft(event.pageX-this._resizerParentOffsetLeft);event.preventDefault();}
_rightResizeElementDragging(event){this._resizeWindowRight(event.pageX-this._resizerParentOffsetLeft);event.preventDefault();}
_startWindowSelectorDragging(event){if(!this._enabled)
return false;this._offsetLeft=this._parentElement.totalOffsetLeft();var position=event.x-this._offsetLeft;this._overviewWindowSelector=new UI.OverviewGrid.WindowSelector(this._parentElement,position);return true;}
_windowSelectorDragging(event){this._overviewWindowSelector._updatePosition(event.x-this._offsetLeft);event.preventDefault();}
_endWindowSelectorDragging(event){var window=this._overviewWindowSelector._close(event.x-this._offsetLeft);delete this._overviewWindowSelector;var clickThreshold=3;if(window.end-window.start<clickThreshold){if(this.dispatchEventToListeners(UI.OverviewGrid.Events.Click,event))
return;var middle=window.end;window.start=Math.max(0,middle-UI.OverviewGrid.MinSelectableSize/2);window.end=Math.min(this._parentElement.clientWidth,middle+UI.OverviewGrid.MinSelectableSize/2);}else if(window.end-window.start<UI.OverviewGrid.MinSelectableSize){if(this._parentElement.clientWidth-window.end>UI.OverviewGrid.MinSelectableSize)
window.end=window.start+UI.OverviewGrid.MinSelectableSize;else
window.start=window.end-UI.OverviewGrid.MinSelectableSize;}
this._setWindowPosition(window.start,window.end);}
_startWindowDragging(event){this._dragStartPoint=event.pageX;this._dragStartLeft=this.windowLeft;this._dragStartRight=this.windowRight;event.stopPropagation();return true;}
_windowDragging(event){event.preventDefault();var delta=(event.pageX-this._dragStartPoint)/this._parentElement.clientWidth;if(this._dragStartLeft+delta<0)
delta=-this._dragStartLeft;if(this._dragStartRight+delta>1)
delta=1-this._dragStartRight;this._setWindow(this._dragStartLeft+delta,this._dragStartRight+delta);}
_resizeWindowLeft(start){if(start<10)
start=0;else if(start>this._rightResizeElement.offsetLeft-4)
start=this._rightResizeElement.offsetLeft-4;this._setWindowPosition(start,null);}
_resizeWindowRight(end){if(end>this._parentElement.clientWidth-10)
end=this._parentElement.clientWidth;else if(end<this._leftResizeElement.offsetLeft+UI.OverviewGrid.MinSelectableSize)
end=this._leftResizeElement.offsetLeft+UI.OverviewGrid.MinSelectableSize;this._setWindowPosition(null,end);}
_resizeWindowMaximum(){this._setWindowPosition(0,this._parentElement.clientWidth);}
_setWindow(windowLeft,windowRight){this.windowLeft=windowLeft;this.windowRight=windowRight;this._updateCurtains();this.dispatchEventToListeners(UI.OverviewGrid.Events.WindowChanged);}
_updateCurtains(){var left=this.windowLeft;var right=this.windowRight;var width=right-left;var widthInPixels=width*this._parentElement.clientWidth;var minWidthInPixels=UI.OverviewGrid.MinSelectableSize/2;if(widthInPixels<minWidthInPixels){var factor=minWidthInPixels/widthInPixels;left=((this.windowRight+this.windowLeft)-width*factor)/2;right=((this.windowRight+this.windowLeft)+width*factor)/2;}
this._leftResizeElement.style.left=(100*left).toFixed(2)+'%';this._rightResizeElement.style.left=(100*right).toFixed(2)+'%';this._leftCurtainElement.style.width=(100*left).toFixed(2)+'%';this._rightCurtainElement.style.width=(100*(1-right)).toFixed(2)+'%';}
_setWindowPosition(start,end){var clientWidth=this._parentElement.clientWidth;var windowLeft=typeof start==='number'?start/clientWidth:this.windowLeft;var windowRight=typeof end==='number'?end/clientWidth:this.windowRight;this._setWindow(windowLeft,windowRight);}
_onMouseWheel(event){if(!this._enabled)
return;if(typeof event.wheelDeltaY==='number'&&event.wheelDeltaY){const zoomFactor=1.1;const mouseWheelZoomSpeed=1/120;var reference=event.offsetX/event.target.clientWidth;this._zoom(Math.pow(zoomFactor,-event.wheelDeltaY*mouseWheelZoomSpeed),reference);}
if(typeof event.wheelDeltaX==='number'&&event.wheelDeltaX){var offset=Math.round(event.wheelDeltaX*UI.OverviewGrid.WindowScrollSpeedFactor);var windowLeft=this._leftResizeElement.offsetLeft+UI.OverviewGrid.ResizerOffset;var windowRight=this._rightResizeElement.offsetLeft+UI.OverviewGrid.ResizerOffset;if(windowLeft-offset<0)
offset=windowLeft;if(windowRight-offset>this._parentElement.clientWidth)
offset=windowRight-this._parentElement.clientWidth;this._setWindowPosition(windowLeft-offset,windowRight-offset);event.preventDefault();}}
_zoom(factor,reference){var left=this.windowLeft;var right=this.windowRight;var windowSize=right-left;var newWindowSize=factor*windowSize;if(newWindowSize>1){newWindowSize=1;factor=newWindowSize/windowSize;}
left=reference+(left-reference)*factor;left=Number.constrain(left,0,1-newWindowSize);right=reference+(right-reference)*factor;right=Number.constrain(right,newWindowSize,1);this._setWindow(left,right);}};UI.OverviewGrid.Events={WindowChanged:Symbol('WindowChanged'),Click:Symbol('Click')};UI.OverviewGrid.WindowSelector=class{constructor(parent,position){this._startPosition=position;this._width=parent.offsetWidth;this._windowSelector=createElement('div');this._windowSelector.className='overview-grid-window-selector';this._windowSelector.style.left=this._startPosition+'px';this._windowSelector.style.right=this._width-this._startPosition+'px';parent.appendChild(this._windowSelector);}
_close(position){position=Math.max(0,Math.min(position,this._width));this._windowSelector.remove();return this._startPosition<position?{start:this._startPosition,end:position}:{start:position,end:this._startPosition};}
_updatePosition(position){position=Math.max(0,Math.min(position,this._width));if(position<this._startPosition){this._windowSelector.style.left=position+'px';this._windowSelector.style.right=this._width-this._startPosition+'px';}else{this._windowSelector.style.left=this._startPosition+'px';this._windowSelector.style.right=this._width-position+'px';}}};;UI.PieChart=class{constructor(size,formatter,showTotal){this.element=createElement('div');this._shadowRoot=UI.createShadowRootWithCoreStyles(this.element,'ui_lazy/pieChart.css');var root=this._shadowRoot.createChild('div','root');var svg=this._createSVGChild(root,'svg');this._group=this._createSVGChild(svg,'g');var background=this._createSVGChild(this._group,'circle');background.setAttribute('r',1.01);background.setAttribute('fill','hsl(0, 0%, 90%)');this._foregroundElement=root.createChild('div','pie-chart-foreground');if(showTotal)
this._totalElement=this._foregroundElement.createChild('div','pie-chart-total');this._formatter=formatter;this._slices=[];this._lastAngle=-Math.PI/2;this._setSize(size);}
setTotal(totalValue){for(var i=0;i<this._slices.length;++i)
this._slices[i].remove();this._slices=[];this._totalValue=totalValue;var totalString;if(totalValue)
totalString=this._formatter?this._formatter(totalValue):totalValue;else
totalString='';if(this._totalElement)
this._totalElement.textContent=totalString;}
_setSize(value){this._group.setAttribute('transform','scale('+(value/2)+') translate(1, 1) scale(0.99, 0.99)');var size=value+'px';this.element.style.width=size;this.element.style.height=size;}
addSlice(value,color){var sliceAngle=value/this._totalValue*2*Math.PI;if(!isFinite(sliceAngle))
return;sliceAngle=Math.min(sliceAngle,2*Math.PI*0.9999);var path=this._createSVGChild(this._group,'path');var x1=Math.cos(this._lastAngle);var y1=Math.sin(this._lastAngle);this._lastAngle+=sliceAngle;var x2=Math.cos(this._lastAngle);var y2=Math.sin(this._lastAngle);var largeArc=sliceAngle>Math.PI?1:0;path.setAttribute('d','M0,0 L'+x1+','+y1+' A1,1,0,'+largeArc+',1,'+x2+','+y2+' Z');path.setAttribute('fill',color);this._slices.push(path);}
_createSVGChild(parent,childType){var child=parent.ownerDocument.createElementNS('http://www.w3.org/2000/svg',childType);parent.appendChild(child);return child;}};;UI.TimelineGrid=class{constructor(){this.element=createElement('div');UI.appendStyle(this.element,'ui_lazy/timelineGrid.css');this._dividersElement=this.element.createChild('div','resources-dividers');this._gridHeaderElement=createElement('div');this._gridHeaderElement.classList.add('timeline-grid-header');this._eventDividersElement=this._gridHeaderElement.createChild('div','resources-event-dividers');this._dividersLabelBarElement=this._gridHeaderElement.createChild('div','resources-dividers-label-bar');this.element.appendChild(this._gridHeaderElement);}
static calculateDividerOffsets(calculator,freeZoneAtLeft){var minGridSlicePx=64;var clientWidth=calculator.computePosition(calculator.maximumBoundary());var dividersCount=clientWidth/minGridSlicePx;var gridSliceTime=calculator.boundarySpan()/dividersCount;var pixelsPerTime=clientWidth/calculator.boundarySpan();var logGridSliceTime=Math.ceil(Math.log(gridSliceTime)/Math.LN10);gridSliceTime=Math.pow(10,logGridSliceTime);if(gridSliceTime*pixelsPerTime>=5*minGridSlicePx)
gridSliceTime=gridSliceTime/5;if(gridSliceTime*pixelsPerTime>=2*minGridSlicePx)
gridSliceTime=gridSliceTime/2;var leftBoundaryTime=calculator.minimumBoundary()-calculator.paddingLeft()/pixelsPerTime;var firstDividerTime=Math.ceil((leftBoundaryTime-calculator.zeroTime())/gridSliceTime)*gridSliceTime+calculator.zeroTime();var lastDividerTime=calculator.maximumBoundary();lastDividerTime+=minGridSlicePx/pixelsPerTime;dividersCount=Math.ceil((lastDividerTime-firstDividerTime)/gridSliceTime);if(!gridSliceTime)
dividersCount=0;var offsets=[];for(var i=0;i<dividersCount;++i){var time=firstDividerTime+gridSliceTime*i;if(calculator.computePosition(time)<freeZoneAtLeft)
continue;offsets.push(time);}
return{offsets:offsets,precision:Math.max(0,-Math.floor(Math.log(gridSliceTime*1.01)/Math.LN10))};}
static drawCanvasGrid(context,calculator,paddingTop,freeZoneAtLeft){context.save();var ratio=window.devicePixelRatio;context.scale(ratio,ratio);var width=context.canvas.width/window.devicePixelRatio;var height=context.canvas.height/window.devicePixelRatio;var dividersData=UI.TimelineGrid.calculateDividerOffsets(calculator);var dividerOffsets=dividersData.offsets;var precision=dividersData.precision;context.fillStyle='rgba(255, 255, 255, 0.5)';context.fillRect(0,0,width,15);context.fillStyle='#333';context.strokeStyle='rgba(0, 0, 0, 0.1)';context.textBaseline='hanging';context.font='11px '+Host.fontFamily();context.lineWidth=1;context.translate(0.5,0.5);const paddingRight=4;freeZoneAtLeft=freeZoneAtLeft||0;for(var i=0;i<dividerOffsets.length;++i){var time=dividerOffsets[i];var position=calculator.computePosition(time);var text=calculator.formatValue(time,precision);var textWidth=context.measureText(text).width;var textPosition=position-textWidth-paddingRight;if(freeZoneAtLeft<textPosition)
context.fillText(text,textPosition,paddingTop);context.moveTo(position,0);context.lineTo(position,height);}
context.stroke();context.restore();}
get dividersElement(){return this._dividersElement;}
get dividersLabelBarElement(){return this._dividersLabelBarElement;}
removeDividers(){this._dividersElement.removeChildren();this._dividersLabelBarElement.removeChildren();}
updateDividers(calculator,freeZoneAtLeft){var dividersData=UI.TimelineGrid.calculateDividerOffsets(calculator,freeZoneAtLeft);var dividerOffsets=dividersData.offsets;var precision=dividersData.precision;var dividersElementClientWidth=this._dividersElement.clientWidth;var divider=(this._dividersElement.firstChild);var dividerLabelBar=(this._dividersLabelBarElement.firstChild);for(var i=0;i<dividerOffsets.length;++i){if(!divider){divider=createElement('div');divider.className='resources-divider';this._dividersElement.appendChild(divider);dividerLabelBar=createElement('div');dividerLabelBar.className='resources-divider';var label=createElement('div');label.className='resources-divider-label';dividerLabelBar._labelElement=label;dividerLabelBar.appendChild(label);this._dividersLabelBarElement.appendChild(dividerLabelBar);}
var time=dividerOffsets[i];var position=calculator.computePosition(time);dividerLabelBar._labelElement.textContent=calculator.formatValue(time,precision);var percentLeft=100*position/dividersElementClientWidth;divider.style.left=percentLeft+'%';dividerLabelBar.style.left=percentLeft+'%';divider=(divider.nextSibling);dividerLabelBar=(dividerLabelBar.nextSibling);}
while(divider){var nextDivider=divider.nextSibling;this._dividersElement.removeChild(divider);divider=nextDivider;}
while(dividerLabelBar){var nextDivider=dividerLabelBar.nextSibling;this._dividersLabelBarElement.removeChild(dividerLabelBar);dividerLabelBar=nextDivider;}
return true;}
addEventDivider(divider){this._eventDividersElement.appendChild(divider);}
addEventDividers(dividers){this._gridHeaderElement.removeChild(this._eventDividersElement);for(var divider of dividers)
this._eventDividersElement.appendChild(divider);this._gridHeaderElement.appendChild(this._eventDividersElement);}
removeEventDividers(){this._eventDividersElement.removeChildren();}
hideEventDividers(){this._eventDividersElement.classList.add('hidden');}
showEventDividers(){this._eventDividersElement.classList.remove('hidden');}
hideDividers(){this._dividersElement.classList.add('hidden');}
showDividers(){this._dividersElement.classList.remove('hidden');}
setScrollTop(scrollTop){this._dividersLabelBarElement.style.top=scrollTop+'px';this._eventDividersElement.style.top=scrollTop+'px';}};UI.TimelineGrid.Calculator=function(){};UI.TimelineGrid.Calculator.prototype={paddingLeft:function(){},computePosition:function(time){},formatValue:function(time,precision){},minimumBoundary:function(){},zeroTime:function(){},maximumBoundary:function(){},boundarySpan:function(){}};;UI.TimelineOverviewPane=class extends UI.VBox{constructor(prefix){super();this.element.id=prefix+'-overview-pane';this._overviewCalculator=new UI.TimelineOverviewCalculator();this._overviewGrid=new UI.OverviewGrid(prefix);this.element.appendChild(this._overviewGrid.element);this._cursorArea=this._overviewGrid.element.createChild('div','overview-grid-cursor-area');this._cursorElement=this._overviewGrid.element.createChild('div','overview-grid-cursor-position');this._cursorArea.addEventListener('mousemove',this._onMouseMove.bind(this),true);this._cursorArea.addEventListener('mouseleave',this._hideCursor.bind(this),true);this._overviewGrid.setResizeEnabled(false);this._overviewGrid.addEventListener(UI.OverviewGrid.Events.WindowChanged,this._onWindowChanged,this);this._overviewGrid.addEventListener(UI.OverviewGrid.Events.Click,this._onClick,this);this._overviewControls=[];this._markers=new Map();this._popoverHelper=new UI.PopoverHelper(this._cursorArea);this._popoverHelper.initializeCallbacks(this._getPopoverAnchor.bind(this),this._showPopover.bind(this),this._onHidePopover.bind(this));this._popoverHelper.setTimeout(0);this._updateThrottler=new Common.Throttler(100);this._cursorEnabled=false;this._cursorPosition=0;this._lastWidth=0;}
_getPopoverAnchor(element,event){return this._cursorArea;}
_showPopover(anchor,popover){this._buildPopoverContents().then(maybeShowPopover.bind(this));function maybeShowPopover(fragment){if(!fragment.firstChild)
return;var content=new UI.TimelineOverviewPane.PopoverContents();this._popoverContents=content.contentElement.createChild('div');this._popoverContents.appendChild(fragment);this._popover=popover;popover.showView(content,this._cursorElement);}}
_onHidePopover(){this._popover=null;this._popoverContents=null;}
_onMouseMove(event){if(!this._cursorEnabled)
return;this._cursorPosition=event.offsetX+event.target.offsetLeft;this._cursorElement.style.left=this._cursorPosition+'px';this._cursorElement.style.visibility='visible';if(!this._popover)
return;this._buildPopoverContents().then(updatePopover.bind(this));this._popover.positionElement(this._cursorElement);function updatePopover(fragment){if(!this._popoverContents)
return;this._popoverContents.removeChildren();this._popoverContents.appendChild(fragment);}}
_buildPopoverContents(){var document=this.element.ownerDocument;var x=this._cursorPosition;var promises=this._overviewControls.map(control=>control.popoverElementPromise(x));return Promise.all(promises).then(buildFragment);function buildFragment(elements){var fragment=document.createDocumentFragment();elements.remove(null);fragment.appendChildren.apply(fragment,elements);return fragment;}}
_hideCursor(){this._cursorElement.style.visibility='hidden';}
wasShown(){this._update();}
willHide(){this._popoverHelper.hidePopover();}
onResize(){var width=this.element.offsetWidth;if(width===this._lastWidth)
return;this._lastWidth=width;this.scheduleUpdate();}
setOverviewControls(overviewControls){for(var i=0;i<this._overviewControls.length;++i)
this._overviewControls[i].dispose();for(var i=0;i<overviewControls.length;++i){overviewControls[i].setCalculator(this._overviewCalculator);overviewControls[i].show(this._overviewGrid.element);}
this._overviewControls=overviewControls;this._update();}
setBounds(minimumBoundary,maximumBoundary){this._overviewCalculator.setBounds(minimumBoundary,maximumBoundary);this._overviewGrid.setResizeEnabled(true);this._cursorEnabled=true;}
scheduleUpdate(){this._updateThrottler.schedule(process.bind(this));function process(){this._update();return Promise.resolve();}}
_update(){if(!this.isShowing())
return;this._overviewCalculator.setDisplayWindow(this._overviewGrid.clientWidth());for(var i=0;i<this._overviewControls.length;++i)
this._overviewControls[i].update();this._overviewGrid.updateDividers(this._overviewCalculator);this._updateMarkers();this._updateWindow();}
setMarkers(markers){this._markers=markers;this._updateMarkers();}
_updateMarkers(){var filteredMarkers=new Map();for(var time of this._markers.keys()){var marker=this._markers.get(time);var position=Math.round(this._overviewCalculator.computePosition(time));if(filteredMarkers.has(position))
continue;filteredMarkers.set(position,marker);marker.style.left=position+'px';}
this._overviewGrid.removeEventDividers();this._overviewGrid.addEventDividers(filteredMarkers.valuesArray());}
reset(){this._windowStartTime=0;this._windowEndTime=Infinity;this._overviewCalculator.reset();this._overviewGrid.reset();this._overviewGrid.setResizeEnabled(false);this._overviewGrid.updateDividers(this._overviewCalculator);this._cursorEnabled=false;this._hideCursor();this._markers=new Map();for(var i=0;i<this._overviewControls.length;++i)
this._overviewControls[i].reset();this._popoverHelper.hidePopover();this._update();}
_onClick(event){var domEvent=(event.data);for(var overviewControl of this._overviewControls){if(overviewControl.onClick(domEvent)){event.preventDefault();return;}}}
_onWindowChanged(event){if(this._muteOnWindowChanged)
return;if(!this._overviewControls.length)
return;var windowTimes=this._overviewControls[0].windowTimes(this._overviewGrid.windowLeft(),this._overviewGrid.windowRight());this._windowStartTime=windowTimes.startTime;this._windowEndTime=windowTimes.endTime;this.dispatchEventToListeners(UI.TimelineOverviewPane.Events.WindowChanged,windowTimes);}
requestWindowTimes(startTime,endTime){if(startTime===this._windowStartTime&&endTime===this._windowEndTime)
return;this._windowStartTime=startTime;this._windowEndTime=endTime;this._updateWindow();this.dispatchEventToListeners(UI.TimelineOverviewPane.Events.WindowChanged,{startTime:startTime,endTime:endTime});}
_updateWindow(){if(!this._overviewControls.length)
return;var windowBoundaries=this._overviewControls[0].windowBoundaries(this._windowStartTime,this._windowEndTime);this._muteOnWindowChanged=true;this._overviewGrid.setWindow(windowBoundaries.left,windowBoundaries.right);this._muteOnWindowChanged=false;}};UI.TimelineOverviewPane.Events={WindowChanged:Symbol('WindowChanged')};UI.TimelineOverviewPane.PopoverContents=class extends UI.VBox{constructor(){super(true);this.contentElement.classList.add('timeline-overview-popover');}};UI.TimelineOverviewCalculator=class{constructor(){this.reset();}
paddingLeft(){return this._paddingLeft;}
computePosition(time){return(time-this._minimumBoundary)/this.boundarySpan()*this._workingArea+this._paddingLeft;}
positionToTime(position){return(position-this._paddingLeft)/this._workingArea*this.boundarySpan()+this._minimumBoundary;}
setBounds(minimumBoundary,maximumBoundary){this._minimumBoundary=minimumBoundary;this._maximumBoundary=maximumBoundary;}
setDisplayWindow(clientWidth,paddingLeft){this._paddingLeft=paddingLeft||0;this._workingArea=clientWidth-this._paddingLeft;}
reset(){this.setBounds(0,100);}
formatValue(value,precision){return Number.preciseMillisToString(value-this.zeroTime(),precision);}
maximumBoundary(){return this._maximumBoundary;}
minimumBoundary(){return this._minimumBoundary;}
zeroTime(){return this._minimumBoundary;}
boundarySpan(){return this._maximumBoundary-this._minimumBoundary;}};UI.TimelineOverview=function(){};UI.TimelineOverview.prototype={show:function(parentElement,insertBefore){},update:function(){},dispose:function(){},reset:function(){},popoverElementPromise:function(x){},onClick:function(event){},windowTimes:function(windowLeft,windowRight){},windowBoundaries:function(startTime,endTime){},timelineStarted:function(){},timelineStopped:function(){},};UI.TimelineOverviewBase=class extends UI.VBox{constructor(){super();this._calculator=null;this._canvas=this.element.createChild('canvas','fill');this._context=this._canvas.getContext('2d');}
update(){this.resetCanvas();}
dispose(){this.detach();}
reset(){}
popoverElementPromise(x){return Promise.resolve((null));}
timelineStarted(){}
timelineStopped(){}
setCalculator(calculator){this._calculator=calculator;}
onClick(event){return false;}
windowTimes(windowLeft,windowRight){var absoluteMin=this._calculator.minimumBoundary();var timeSpan=this._calculator.maximumBoundary()-absoluteMin;return{startTime:absoluteMin+timeSpan*windowLeft,endTime:absoluteMin+timeSpan*windowRight};}
windowBoundaries(startTime,endTime){var absoluteMin=this._calculator.minimumBoundary();var timeSpan=this._calculator.maximumBoundary()-absoluteMin;var haveRecords=absoluteMin>0;return{left:haveRecords&&startTime?Math.min((startTime-absoluteMin)/timeSpan,1):0,right:haveRecords&&endTime<Infinity?(endTime-absoluteMin)/timeSpan:1};}
resetCanvas(){this._canvas.width=this.element.clientWidth*window.devicePixelRatio;this._canvas.height=this.element.clientHeight*window.devicePixelRatio;}};;Runtime.cachedResources["ui_lazy/dataGrid.css"]=".data-grid {\n    position: relative;\n    border: 1px solid #aaa;\n    font-size: 11px;\n    line-height: 120%;\n}\n\n.data-grid table {\n    table-layout: fixed;\n    border-spacing: 0;\n    border-collapse: separate;\n    height: 100%;\n    width: 100%;\n}\n\n.data-grid .header-container,\n.data-grid .data-container {\n    position: absolute;\n    left: 0;\n    right: 0;\n    overflow-x: hidden;\n}\n\n.data-grid .header-container {\n    top: 0;\n    height: 17px;\n }\n\n.data-grid .data-container {\n    top: 17px;\n    bottom: 0;\n    overflow-y: overlay;\n    transform: translateZ(0);\n}\n\n.data-grid.inline .header-container,\n.data-grid.inline .data-container {\n    position: static;\n}\n\n.data-grid.inline .corner {\n    display: none;\n}\n\n.platform-mac .data-grid .corner,\n.data-grid.data-grid-fits-viewport .corner {\n    display: none;\n}\n\n.data-grid .corner {\n    width: 14px;\n    padding-right: 0;\n    padding-left: 0;\n    border-left: 0 none transparent !important;\n}\n\n.data-grid .top-filler-td,\n.data-grid .bottom-filler-td {\n    height: auto !important;\n    padding: 0 !important;\n}\n\n.data-grid table.data {\n    position: absolute;\n    left: 0;\n    top: 0;\n    right: 0;\n    bottom: 0;\n    border-top: 0 none transparent;\n    background-image: linear-gradient(to bottom, transparent, transparent 50%, hsla(214, 100%, 40%, 0.1) 50%, hsla(214, 100%, 40%, 0.1));\n    background-size: 128px 32px;\n    table-layout: fixed;\n}\n\n.data-grid.inline table.data {\n    position: static;\n}\n\n.data-grid table.data tr {\n    display: none;\n}\n\n.data-grid table.data tr.revealed {\n    display: table-row;\n}\n\n.data-grid td,\n.data-grid th {\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    line-height: 14px;\n    border-left: 1px solid #aaa;\n}\n\n.data-grid th:first-child,\n.data-grid td:first-child {\n    border-left: none !important;\n}\n\n.data-grid td {\n    height: 16px; /* Keep in sync with .data-grid table.data @ background-size */\n    vertical-align: top;\n    padding: 1px 4px;\n    -webkit-user-select: text;\n}\n\n.data-grid th {\n    height: auto;\n    text-align: left;\n    background-color: #eee;\n    border-bottom: 1px solid #aaa;\n    font-weight: normal;\n    vertical-align: middle;\n    padding: 0 4px;\n}\n\n.data-grid td > div,\n.data-grid th > div {\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n}\n\n.data-grid td.editing > div {\n    text-overflow: clip;\n}\n\n.data-grid .center {\n    text-align: center;\n}\n\n.data-grid .right {\n    text-align: right;\n}\n\n.data-grid th.sortable {\n    position: relative;\n}\n\n.data-grid th.sortable:active::after {\n    content: \"\";\n    position: absolute;\n    left: 0;\n    right: 0;\n    top: 0;\n    bottom: 0;\n    background-color: rgba(0, 0, 0, 0.15);\n}\n\n.data-grid th .sort-order-icon-container {\n    position: absolute;\n    top: 1px;\n    right: 0;\n    bottom: 1px;\n    display: flex;\n    align-items: center;\n}\n\n.data-grid th .sort-order-icon {\n    margin-right: 4px;\n    background-image: url(Images/toolbarButtonGlyphs.png);\n    background-size: 352px 168px;\n    opacity: 0.5;\n    width: 8px;\n    height: 7px;\n    display: none;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.1) {\n.data-grid th .sort-order-icon {\n    background-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.data-grid th.sort-ascending .sort-order-icon {\n    display: block;\n    background-position: -4px -111px;\n}\n\n.data-grid th.sort-descending .sort-order-icon {\n    display: block;\n    background-position: -20px -99px;\n}\n\n.data-grid th:hover {\n    background-color: hsla(0, 0%, 90%, 1);\n}\n\n.data-grid button {\n    line-height: 18px;\n    color: inherit;\n}\n\n.data-grid td.disclosure::before {\n    -webkit-user-select: none;\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs.png);\n    -webkit-mask-position: -4px -96px;\n    -webkit-mask-size: 352px 168px;\n    float: left;\n    width: 8px;\n    height: 12px;\n    margin-right: 2px;\n    content: \"a\";\n    color: transparent;\n    position: relative;\n    top: 1px;\n    background-color: rgb(110, 110, 110);\n}\n\n.data-grid tr:not(.parent) td.disclosure::before {\n    background-color: transparent;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.1) {\n.data-grid tr.parent td.disclosure::before {\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.data-grid tr.expanded td.disclosure::before {\n    -webkit-mask-position: -20px -96px;\n}\n\n.data-grid tr.selected {\n    background-color: rgb(212, 212, 212);\n    color: inherit;\n}\n\n.data-grid:focus tr.selected {\n    background-color: rgb(56, 121, 217);\n    color: white;\n}\n\n.data-grid:focus tr.selected a {\n    color: white;\n}\n\n.data-grid:focus tr.parent.selected td.disclosure::before {\n    background-color: white;\n    -webkit-mask-position: -4px -96px;\n}\n\n.data-grid:focus tr.expanded.selected td.disclosure::before {\n    background-color: white;\n    -webkit-mask-position: -20px -96px;\n}\n\n.data-grid-resizer {\n    position: absolute;\n    top: 0;\n    bottom: 0;\n    width: 5px;\n    z-index: 500;\n}\n\n/*# sourceURL=ui_lazy/dataGrid.css */";Runtime.cachedResources["ui_lazy/dialog.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n:host {\n    padding: 10px;\n}\n\n.widget {\n    align-items: center;\n}\n\nlabel {\n    white-space: nowrap;\n}\n\ninput[type=text] {\n    font-size: inherit;\n    height: 24px;\n    padding-left: 2px;\n    margin: 0 5px;\n}\n\ninput[type=\"search\"]:focus,\ninput[type=\"text\"]:focus {\n    outline: none;\n}\n\nbutton {\n    background-image: linear-gradient(hsl(0, 0%, 93%), hsl(0, 0%, 93%) 38%, hsl(0, 0%, 87%));\n    border: 1px solid hsla(0, 0%, 0%, 0.25);\n    border-radius: 2px;\n    box-shadow: 0 1px 0 hsla(0, 0%, 0%, 0.08), inset 0 1px 2px hsla(0, 100%, 100%, 0.75);\n    color: hsl(0, 0%, 27%);\n    font-size: 12px;\n    margin: 0 6px 0 6px;\n    text-shadow: 0 1px 0 hsl(0, 0%, 94%);\n    min-height: 2em;\n    padding-left: 10px;\n    padding-right: 10px;\n}\n\nbutton:active {\n    background-color: rgb(215, 215, 215);\n    background-image: linear-gradient(to bottom, rgb(194, 194, 194), rgb(239, 239, 239));\n}\n\n/*# sourceURL=ui_lazy/dialog.css */";Runtime.cachedResources["ui_lazy/filteredListWidget.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.filtered-list-widget {\n    display: flex;\n    flex-direction: column;\n    flex: auto;\n}\n\n.filtered-list-widget-prompt-element {\n    flex: 0 0 36px;\n    border: 0;\n    border-bottom:1px solid rgba(0, 0, 0, 0.2);\n    margin: 0;\n    padding: 0 6px;\n    z-index: 1;\n    font-size: inherit;\n}\n\n.filtered-list-widget-input {\n    white-space: pre;\n    height: 18px;\n    margin-top: 10px;\n    overflow: hidden;\n}\n\n.filtered-list-widget > div.container {\n    flex: auto;\n    overflow-y: auto;\n    background: #fbfbfb;\n}\n\n.filtered-list-widget-item {\n    padding: 4px 6px;\n    white-space: nowrap;\n    text-overflow: ellipsis;\n    overflow: hidden;\n    color: rgb(95, 95, 95);\n}\n\n.filtered-list-widget-item.selected {\n    background-color: #f0f0f0;\n}\n\n.filtered-list-widget-item span.highlight {\n    color: #222;\n    font-weight: bold;\n}\n\n.filtered-list-widget-item .filtered-list-widget-title {\n    flex: auto;\n    overflow: hidden;\n    text-overflow: ellipsis;\n}\n\n.filtered-list-widget-item .filtered-list-widget-subtitle {\n    flex: none;\n    overflow: hidden;\n    text-overflow: ellipsis;\n    color: rgb(155, 155, 155);\n    display: flex;\n}\n\n.filtered-list-widget-item .filtered-list-widget-subtitle .first-part {\n    flex-shrink: 1000;\n    overflow: hidden;\n    text-overflow: ellipsis;\n}\n\n.filtered-list-widget-item.one-row {\n    display: flex;\n}\n\n.filtered-list-widget-item.two-rows {\n    border-bottom: 1px solid rgb(235, 235, 235);\n}\n\n.tag {\n    color: white;\n    padding: 1px 3px;\n    margin-right: 5px;\n    border-radius: 2px;\n    line-height: 18px;\n}\n\n.filtered-list-widget-item .tag .highlight {\n    color: white;\n}\n\n/*# sourceURL=ui_lazy/filteredListWidget.css */";Runtime.cachedResources["ui_lazy/flameChart.css"]=".flame-chart-main-pane {\n    overflow: hidden;\n}\n\n.flame-chart-marker-highlight-element {\n    position: absolute;\n    top: 0;\n    height: 20px;\n    width: 4px;\n    margin: 0 -2px;\n    content: \"\";\n    display: block;\n}\n\n.flame-chart-highlight-element {\n    background-color: black;\n    position: absolute;\n    opacity: 0.2;\n    pointer-events: none;\n}\n\n.flame-chart-selected-element {\n    position: absolute;\n    pointer-events: none;\n    border-color: rgb(56, 121, 217);\n    border-width: 1px;\n    border-style: solid;\n    background-color: rgba(56, 121, 217, 0.2);\n}\n\n.flame-chart-v-scroll {\n    position: absolute;\n    top: 0;\n    right: 0;\n    bottom: 0;\n    overflow-x: hidden;\n    z-index: 200;\n    padding-left: 1px;\n}\n\n:host-context(.platform-mac) .flame-chart-v-scroll {\n    right: 2px;\n    top: 3px;\n    bottom: 3px;\n}\n\n/* force non-overlay scrollbars */\n:host-context(.platform-mac) ::-webkit-scrollbar {\n    width: 8px;\n}\n\n:host-context(.platform-mac) ::-webkit-scrollbar-thumb {\n    background-color: hsla(0, 0%, 56%, 0.6);\n    border-radius: 50px;\n}\n\n:host-context(.platform-mac) .flame-chart-v-scroll:hover::-webkit-scrollbar-thumb {\n    background-color: hsla(0, 0%, 25%, 0.6);\n}\n\n.flame-chart-selection-overlay {\n    position: absolute;\n    z-index: 100;\n    background-color: rgba(56, 121, 217, 0.3);\n    border-color: rgb(16, 81, 177);\n    border-width: 0 1px;\n    border-style: solid;\n    pointer-events: none;\n    top: 0;\n    bottom: 0;\n    text-align: center;\n}\n\n.flame-chart-selection-overlay .time-span {\n    white-space: nowrap;\n    position: absolute;\n    left: 0;\n    right: 0;\n    bottom: 0;\n}\n\n.flame-chart-entry-info:not(:empty) {\n    z-index: 2000;\n    position: absolute;\n    background-color: white;\n    pointer-events: none;\n    padding: 4px 8px;\n    white-space: nowrap;\n    max-width: 80%;\n    box-shadow: 0 0 0 1px rgba(0, 0, 0, 0.05),\n                0 2px 4px rgba(0, 0, 0, 0.2),\n                0 2px 6px rgba(0, 0, 0, 0.1);\n}\n\n.flame-chart-entry-info table tr td:empty {\n    padding: 0;\n}\n\n.flame-chart-entry-info table tr td:not(:empty) {\n    padding: 0 5px;\n    white-space: nowrap;\n}\n\n.flame-chart-entry-info table tr td:first-child {\n    font-weight: bold;\n}\n\n.flame-chart-entry-info table tr td span {\n    margin-right: 5px;\n}\n\n/*# sourceURL=ui_lazy/flameChart.css */";Runtime.cachedResources["ui_lazy/overviewGrid.css"]="/*\n * Copyright (c) 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.overview-grid-window-selector {\n    position: absolute;\n    top: 0;\n    bottom: 0;\n    background-color: rgba(125, 173, 217, 0.5);\n    z-index: 250;\n    pointer-events: none;\n}\n\n.overview-grid-window-resizer {\n    position: absolute;\n    top: -1px;\n    height: 20px;\n    width: 6px;\n    margin-left: -3px;\n    background-color: rgb(153, 153, 153);\n    border: 1px solid white;\n    z-index: 500;\n}\n\n.overview-grid-cursor-area {\n    position: absolute;\n    left: 0;\n    right: 0;\n    top: 20px;\n    bottom: 0;\n    z-index: 500;\n    cursor: text;\n}\n\n.overview-grid-cursor-position {\n    position: absolute;\n    top: 0;\n    bottom: 0;\n    width: 2px;\n    background-color: hsla(220, 95%, 50%, 0.7);\n    z-index: 500;\n    pointer-events: none;\n    visibility: hidden;\n    overflow: hidden;\n}\n\n.window-curtain-left, .window-curtain-right {\n    background-color: hsla(0, 0%, 80%, 0.5);\n    position: absolute;\n    top: 0;\n    height: 100%;\n    z-index: 300;\n    pointer-events: none;\n    border: 1px none hsla(0, 0%, 70%, 0.5);\n}\n\n.window-curtain-left {\n    left: 0;\n    border-right-style: solid;\n}\n\n.window-curtain-right {\n    right: 0;\n    border-left-style: solid;\n}\n\n/*# sourceURL=ui_lazy/overviewGrid.css */";Runtime.cachedResources["ui_lazy/pieChart.css"]="/*\n * Copyright (c) 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.root {\n    position: relative;\n    width: 100%;\n    height: 100%;\n}\n\n.pie-chart-foreground {\n    position: absolute;\n    width: 100%;\n    height: 100%;\n    z-index: 10;\n    top: 0;\n    display: flex;\n}\n\n.pie-chart-total {\n    margin: auto;\n    padding: 2px 5px;\n    background-color: rgba(255, 255, 255, 0.6);\n}\n\n/*# sourceURL=ui_lazy/pieChart.css */";Runtime.cachedResources["ui_lazy/timelineGrid.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.resources-dividers {\n    position: absolute;\n    left: 0;\n    right: 0;\n    top: 0;\n    z-index: -100;\n    bottom: 0;\n}\n\n.resources-event-dividers {\n    position: absolute;\n    left: 0;\n    right: 0;\n    height: 100%;\n    top: 0;\n    z-index: 300;\n    pointer-events: none;\n}\n\n.resources-dividers-label-bar {\n    position: absolute;\n    top: 0;\n    left: 0;\n    right: 0;\n    background-color: rgba(255, 255, 255, 0.85);\n    background-clip: padding-box;\n    height: 20px;\n    z-index: 200;\n    pointer-events: none;\n    overflow: hidden;\n}\n\n.resources-divider {\n    position: absolute;\n    width: 1px;\n    top: 0;\n    bottom: 0;\n    background-color: rgba(0, 0, 0, 0.1);\n}\n\n.resources-event-divider {\n    position: absolute;\n    width: 2px;\n    top: 0;\n    bottom: 0;\n    z-index: 300;\n}\n\n.resources-divider-label {\n    position: absolute;\n    top: 4px;\n    right: 3px;\n    font-size: 80%;\n    white-space: nowrap;\n    pointer-events: none;\n}\n\n.timeline-grid-header {\n    height: 20px;\n    pointer-events: none;\n}\n\n/*# sourceURL=ui_lazy/timelineGrid.css */";