Components.CookiesTable=class extends UI.VBox{constructor(expandable,refreshCallback,selectedCallback){super();var readOnly=expandable;this._refreshCallback=refreshCallback;var columns=([{id:'name',title:Common.UIString('Name'),sortable:true,disclosure:expandable,sort:UI.DataGrid.Order.Ascending,longText:true,weight:24},{id:'value',title:Common.UIString('Value'),sortable:true,longText:true,weight:34},{id:'domain',title:Common.UIString('Domain'),sortable:true,weight:7},{id:'path',title:Common.UIString('Path'),sortable:true,weight:7},{id:'expires',title:Common.UIString('Expires / Max-Age'),sortable:true,weight:7},{id:'size',title:Common.UIString('Size'),sortable:true,align:UI.DataGrid.Align.Right,weight:7},{id:'httpOnly',title:Common.UIString('HTTP'),sortable:true,align:UI.DataGrid.Align.Center,weight:7},{id:'secure',title:Common.UIString('Secure'),sortable:true,align:UI.DataGrid.Align.Center,weight:7},{id:'sameSite',title:Common.UIString('SameSite'),sortable:true,align:UI.DataGrid.Align.Center,weight:7}]);if(readOnly){this._dataGrid=new UI.DataGrid(columns);}else{this._dataGrid=new UI.DataGrid(columns,undefined,this._onDeleteCookie.bind(this),refreshCallback);this._dataGrid.setRowContextMenuCallback(this._onRowContextMenu.bind(this));}
this._dataGrid.setName('cookiesTable');this._dataGrid.addEventListener(UI.DataGrid.Events.SortingChanged,this._rebuildTable,this);if(selectedCallback)
this._dataGrid.addEventListener(UI.DataGrid.Events.SelectedNode,selectedCallback,this);this._nextSelectedCookie=(null);this._dataGrid.asWidget().show(this.element);this._data=[];}
_clearAndRefresh(domain){this.clear(domain);this._refresh();}
_onRowContextMenu(contextMenu,node){if(node===this._dataGrid.creationNode)
return;var domain=node.cookie.domain();if(domain){contextMenu.appendItem(Common.UIString.capitalize('Clear ^all from "%s"',domain),this._clearAndRefresh.bind(this,domain));}
contextMenu.appendItem(Common.UIString.capitalize('Clear ^all'),this._clearAndRefresh.bind(this,null));}
setCookies(cookies){this.setCookieFolders([{cookies:cookies}]);}
setCookieFolders(cookieFolders){this._data=cookieFolders;this._rebuildTable();}
selectedCookie(){var node=this._dataGrid.selectedNode;return node?node.cookie:null;}
clear(domain){for(var i=0,length=this._data.length;i<length;++i){var cookies=this._data[i].cookies;for(var j=0,cookieCount=cookies.length;j<cookieCount;++j){if(!domain||cookies[j].domain()===domain)
cookies[j].remove();}}}
_rebuildTable(){var selectedCookie=this._nextSelectedCookie||this.selectedCookie();this._nextSelectedCookie=null;this._dataGrid.rootNode().removeChildren();for(var i=0;i<this._data.length;++i){var item=this._data[i];if(item.folderName){var groupData={name:item.folderName,value:'',domain:'',path:'',expires:'',size:this._totalSize(item.cookies),httpOnly:'',secure:'',sameSite:''};var groupNode=new UI.DataGridNode(groupData);groupNode.selectable=true;this._dataGrid.rootNode().appendChild(groupNode);groupNode.element().classList.add('row-group');this._populateNode(groupNode,item.cookies,selectedCookie);groupNode.expand();}else{this._populateNode(this._dataGrid.rootNode(),item.cookies,selectedCookie);}}}
_populateNode(parentNode,cookies,selectedCookie){parentNode.removeChildren();if(!cookies)
return;this._sortCookies(cookies);for(var i=0;i<cookies.length;++i){var cookie=cookies[i];var cookieNode=this._createGridNode(cookie);parentNode.appendChild(cookieNode);if(selectedCookie&&selectedCookie.name()===cookie.name()&&selectedCookie.domain()===cookie.domain()&&selectedCookie.path()===cookie.path())
cookieNode.select();}}
_totalSize(cookies){var totalSize=0;for(var i=0;cookies&&i<cookies.length;++i)
totalSize+=cookies[i].size();return totalSize;}
_sortCookies(cookies){var sortDirection=this._dataGrid.isSortOrderAscending()?1:-1;function getValue(cookie,property){return typeof cookie[property]==='function'?String(cookie[property]()):String(cookie.name());}
function compareTo(property,cookie1,cookie2){return sortDirection*getValue(cookie1,property).compareTo(getValue(cookie2,property));}
function numberCompare(cookie1,cookie2){return sortDirection*(cookie1.size()-cookie2.size());}
function expiresCompare(cookie1,cookie2){if(cookie1.session()!==cookie2.session())
return sortDirection*(cookie1.session()?1:-1);if(cookie1.session())
return 0;if(cookie1.maxAge()&&cookie2.maxAge())
return sortDirection*(cookie1.maxAge()-cookie2.maxAge());if(cookie1.expires()&&cookie2.expires())
return sortDirection*(cookie1.expires()-cookie2.expires());return sortDirection*(cookie1.expires()?1:-1);}
var comparator;var columnId=this._dataGrid.sortColumnId()||'name';if(columnId==='expires')
comparator=expiresCompare;else if(columnId==='size')
comparator=numberCompare;else
comparator=compareTo.bind(null,columnId);cookies.sort(comparator);}
_createGridNode(cookie){var data={};data.name=cookie.name();data.value=cookie.value();if(cookie.type()===SDK.Cookie.Type.Request){data.domain=Common.UIString('N/A');data.path=Common.UIString('N/A');data.expires=Common.UIString('N/A');}else{data.domain=cookie.domain()||'';data.path=cookie.path()||'';if(cookie.maxAge())
data.expires=Number.secondsToString(parseInt(cookie.maxAge(),10));else if(cookie.expires())
data.expires=new Date(cookie.expires()).toISOString();else
data.expires=Common.UIString('Session');}
data.size=cookie.size();const checkmark='\u2713';data.httpOnly=(cookie.httpOnly()?checkmark:'');data.secure=(cookie.secure()?checkmark:'');data.sameSite=cookie.sameSite()||'';var node=new UI.DataGridNode(data);node.cookie=cookie;node.selectable=true;return node;}
_onDeleteCookie(node){var cookie=node.cookie;var neighbour=node.traverseNextNode()||node.traversePreviousNode();if(neighbour)
this._nextSelectedCookie=neighbour.cookie;cookie.remove();this._refresh();}
_refresh(){if(this._refreshCallback)
this._refreshCallback();}};;Components.FilmStripModel=class{constructor(tracingModel,zeroTime){this.reset(tracingModel,zeroTime);}
reset(tracingModel,zeroTime){this._zeroTime=zeroTime||tracingModel.minimumRecordTime();this._spanTime=tracingModel.maximumRecordTime()-this._zeroTime;this._frames=[];var browserMain=SDK.TracingModel.browserMainThread(tracingModel);if(!browserMain)
return;var events=browserMain.events();for(var i=0;i<events.length;++i){var event=events[i];if(event.startTime<this._zeroTime)
continue;if(!event.hasCategory(Components.FilmStripModel._category))
continue;if(event.name===Components.FilmStripModel.TraceEvents.CaptureFrame){var data=event.args['data'];if(data)
this._frames.push(Components.FilmStripModel.Frame._fromEvent(this,event,this._frames.length));}else if(event.name===Components.FilmStripModel.TraceEvents.Screenshot){this._frames.push(Components.FilmStripModel.Frame._fromSnapshot(this,(event),this._frames.length));}}}
frames(){return this._frames;}
zeroTime(){return this._zeroTime;}
spanTime(){return this._spanTime;}
frameByTimestamp(timestamp){var index=this._frames.upperBound(timestamp,(timestamp,frame)=>timestamp-frame.timestamp)-1;return index>=0?this._frames[index]:null;}};Components.FilmStripModel._category='disabled-by-default-devtools.screenshot';Components.FilmStripModel.TraceEvents={CaptureFrame:'CaptureFrame',Screenshot:'Screenshot'};Components.FilmStripModel.Frame=class{constructor(model,timestamp,index){this._model=model;this.timestamp=timestamp;this.index=index;this._imageData=null;this._snapshot=null;}
static _fromEvent(model,event,index){var frame=new Components.FilmStripModel.Frame(model,event.startTime,index);frame._imageData=event.args['data'];return frame;}
static _fromSnapshot(model,snapshot,index){var frame=new Components.FilmStripModel.Frame(model,snapshot.startTime,index);frame._snapshot=snapshot;return frame;}
model(){return this._model;}
imageDataPromise(){if(this._imageData||!this._snapshot)
return Promise.resolve(this._imageData);return(this._snapshot.objectPromise());}};;Components.FilmStripView=class extends UI.HBox{constructor(){super(true);this.registerRequiredCSS('components_lazy/filmStripView.css');this.contentElement.classList.add('film-strip-view');this._statusLabel=this.contentElement.createChild('div','label');this.reset();this.setMode(Components.FilmStripView.Modes.TimeBased);}
static _setImageData(imageElement,data){if(data)
imageElement.src='data:image/jpg;base64,'+data;}
setMode(mode){this._mode=mode;this.contentElement.classList.toggle('time-based',mode===Components.FilmStripView.Modes.TimeBased);this.update();}
setModel(filmStripModel,zeroTime,spanTime){this._model=filmStripModel;this._zeroTime=zeroTime;this._spanTime=spanTime;var frames=filmStripModel.frames();if(!frames.length){this.reset();return;}
this.update();}
createFrameElement(frame){var time=frame.timestamp;var element=createElementWithClass('div','frame');element.title=Common.UIString('Doubleclick to zoom image. Click to view preceding requests.');element.createChild('div','time').textContent=Number.millisToString(time-this._zeroTime);var imageElement=element.createChild('div','thumbnail').createChild('img');element.addEventListener('mousedown',this._onMouseEvent.bind(this,Components.FilmStripView.Events.FrameSelected,time),false);element.addEventListener('mouseenter',this._onMouseEvent.bind(this,Components.FilmStripView.Events.FrameEnter,time),false);element.addEventListener('mouseout',this._onMouseEvent.bind(this,Components.FilmStripView.Events.FrameExit,time),false);element.addEventListener('dblclick',this._onDoubleClick.bind(this,frame),false);return frame.imageDataPromise().then(Components.FilmStripView._setImageData.bind(null,imageElement)).then(returnElement);function returnElement(){return element;}}
frameByTime(time){function comparator(time,frame){return time-frame.timestamp;}
var frames=this._model.frames();var index=Math.max(frames.upperBound(time,comparator)-1,0);return frames[index];}
update(){if(!this._model)
return;var frames=this._model.frames();if(!frames.length)
return;if(this._mode===Components.FilmStripView.Modes.FrameBased){Promise.all(frames.map(this.createFrameElement.bind(this))).then(appendElements.bind(this));return;}
var width=this.contentElement.clientWidth;var scale=this._spanTime/width;this.createFrameElement(frames[0]).then(continueWhenFrameImageLoaded.bind(this));function continueWhenFrameImageLoaded(element0){var frameWidth=Math.ceil(UI.measurePreferredSize(element0,this.contentElement).width);if(!frameWidth)
return;var promises=[];for(var pos=frameWidth;pos<width;pos+=frameWidth){var time=pos*scale+this._zeroTime;promises.push(this.createFrameElement(this.frameByTime(time)).then(fixWidth));}
Promise.all(promises).then(appendElements.bind(this));function fixWidth(element){element.style.width=frameWidth+'px';return element;}}
function appendElements(elements){this.contentElement.removeChildren();for(var i=0;i<elements.length;++i)
this.contentElement.appendChild(elements[i]);}}
onResize(){if(this._mode===Components.FilmStripView.Modes.FrameBased)
return;this.update();}
_onMouseEvent(eventName,timestamp){this.dispatchEventToListeners(eventName,timestamp);}
_onDoubleClick(filmStripFrame){new Components.FilmStripView.Dialog(filmStripFrame,this._zeroTime);}
reset(){this._zeroTime=0;this.contentElement.removeChildren();this.contentElement.appendChild(this._statusLabel);}
setStatusText(text){this._statusLabel.textContent=text;}};Components.FilmStripView.Events={FrameSelected:Symbol('FrameSelected'),FrameEnter:Symbol('FrameEnter'),FrameExit:Symbol('FrameExit'),};Components.FilmStripView.Modes={TimeBased:'TimeBased',FrameBased:'FrameBased'};Components.FilmStripView.Dialog=class extends UI.VBox{constructor(filmStripFrame,zeroTime){super(true);this.registerRequiredCSS('components_lazy/filmStripDialog.css');this.contentElement.classList.add('filmstrip-dialog');this.contentElement.tabIndex=0;this._frames=filmStripFrame.model().frames();this._index=filmStripFrame.index;this._zeroTime=zeroTime||filmStripFrame.model().zeroTime();this._imageElement=this.contentElement.createChild('img');var footerElement=this.contentElement.createChild('div','filmstrip-dialog-footer');footerElement.createChild('div','flex-auto');var prevButton=createTextButton('\u25C0',this._onPrevFrame.bind(this),undefined,Common.UIString('Previous frame'));footerElement.appendChild(prevButton);this._timeLabel=footerElement.createChild('div','filmstrip-dialog-label');var nextButton=createTextButton('\u25B6',this._onNextFrame.bind(this),undefined,Common.UIString('Next frame'));footerElement.appendChild(nextButton);footerElement.createChild('div','flex-auto');this.contentElement.addEventListener('keydown',this._keyDown.bind(this),false);this.setDefaultFocusedElement(this.contentElement);this._render();}
_resize(){if(!this._dialog){this._dialog=new UI.Dialog();this.show(this._dialog.element);this._dialog.setWrapsContent(true);this._dialog.show();}
this._dialog.contentResized();}
_keyDown(event){switch(event.key){case'ArrowLeft':if(Host.isMac()&&event.metaKey)
this._onFirstFrame();else
this._onPrevFrame();break;case'ArrowRight':if(Host.isMac()&&event.metaKey)
this._onLastFrame();else
this._onNextFrame();break;case'Home':this._onFirstFrame();break;case'End':this._onLastFrame();break;}}
_onPrevFrame(){if(this._index>0)
--this._index;this._render();}
_onNextFrame(){if(this._index<this._frames.length-1)
++this._index;this._render();}
_onFirstFrame(){this._index=0;this._render();}
_onLastFrame(){this._index=this._frames.length-1;this._render();}
_render(){var frame=this._frames[this._index];this._timeLabel.textContent=Number.millisToString(frame.timestamp-this._zeroTime);return frame.imageDataPromise().then(Components.FilmStripView._setImageData.bind(null,this._imageElement)).then(this._resize.bind(this));}};;Components.LineLevelProfile=class{constructor(){this._locationPool=new Bindings.LiveLocationPool();this.reset();}
static instance(){if(!Components.LineLevelProfile._instance)
Components.LineLevelProfile._instance=new Components.LineLevelProfile();return Components.LineLevelProfile._instance;}
appendCPUProfile(profile){var nodesToGo=[profile.profileHead];var sampleDuration=(profile.profileEndTime-profile.profileStartTime)/profile.totalHitCount;while(nodesToGo.length){var nodes=nodesToGo.pop().children;for(var i=0;i<nodes.length;++i){var node=nodes[i];nodesToGo.push(node);if(!node.url||!node.positionTicks)
continue;var fileInfo=this._files.get(node.url);if(!fileInfo){fileInfo=new Map();this._files.set(node.url,fileInfo);}
for(var j=0;j<node.positionTicks.length;++j){var lineInfo=node.positionTicks[j];var line=lineInfo.line;var time=lineInfo.ticks*sampleDuration;fileInfo.set(line,(fileInfo.get(line)||0)+time);}}}
this._scheduleUpdate();}
reset(){this._files=new Map();this._scheduleUpdate();}
_scheduleUpdate(){if(this._updateTimer)
return;this._updateTimer=setTimeout(()=>{this._updateTimer=null;this._doUpdate();},0);}
_doUpdate(){this._locationPool.disposeAll();Workspace.workspace.uiSourceCodes().forEach(uiSourceCode=>uiSourceCode.removeAllLineDecorations(Components.LineLevelProfile.LineDecorator.type));for(var fileInfo of this._files){var url=(fileInfo[0]);var uiSourceCode=Workspace.workspace.uiSourceCodeForURL(url);if(!uiSourceCode)
continue;var target=Bindings.NetworkProject.targetForUISourceCode(uiSourceCode)||SDK.targetManager.mainTarget();var debuggerModel=target?SDK.DebuggerModel.fromTarget(target):null;if(!debuggerModel)
continue;for(var lineInfo of fileInfo[1]){var line=lineInfo[0]-1;var time=lineInfo[1];var rawLocation=debuggerModel.createRawLocationByURL(url,line,0);if(rawLocation)
new Components.LineLevelProfile.Presentation(rawLocation,time,this._locationPool);else if(uiSourceCode)
uiSourceCode.addLineDecoration(line,Components.LineLevelProfile.LineDecorator.type,time);}}}};Components.LineLevelProfile.Presentation=class{constructor(rawLocation,time,locationPool){this._time=time;Bindings.debuggerWorkspaceBinding.createLiveLocation(rawLocation,this.updateLocation.bind(this),locationPool);}
updateLocation(liveLocation){if(this._uiLocation){this._uiLocation.uiSourceCode.removeLineDecoration(this._uiLocation.lineNumber,Components.LineLevelProfile.LineDecorator.type);}
this._uiLocation=liveLocation.uiLocation();if(this._uiLocation){this._uiLocation.uiSourceCode.addLineDecoration(this._uiLocation.lineNumber,Components.LineLevelProfile.LineDecorator.type,this._time);}}};Components.LineLevelProfile.LineDecorator=class{decorate(uiSourceCode,textEditor){var gutterType='CodeMirror-gutter-performance';var decorations=uiSourceCode.lineDecorations(Components.LineLevelProfile.LineDecorator.type);textEditor.uninstallGutter(gutterType);if(!decorations)
return;textEditor.installGutter(gutterType,false);for(var decoration of decorations.values()){var time=(decoration.data());var text=Common.UIString('%.1f\xa0ms',time);var intensity=Number.constrain(Math.log10(1+2*time)/5,0.02,1);var element=createElementWithClass('div','text-editor-line-marker-performance');element.textContent=text;element.style.backgroundColor=`hsla(44, 100%, 50%, ${intensity.toFixed(3)})`;textEditor.setGutterDecoration(decoration.line(),gutterType,element);}}};Components.LineLevelProfile.LineDecorator.type='performance';;Components.CoverageProfile=class{constructor(){this._updateTimer=null;this.reset();}
static instance(){if(!Components.CoverageProfile._instance)
Components.CoverageProfile._instance=new Components.CoverageProfile();return Components.CoverageProfile._instance;}
appendUnusedRule(url,range){if(!url)
return;var uiSourceCode=Workspace.workspace.uiSourceCodeForURL(url);if(!uiSourceCode)
return;for(var line=range.startLine;line<=range.endLine;++line)
uiSourceCode.addLineDecoration(line,Components.CoverageProfile.LineDecorator.type,range.startColumn);}
reset(){Workspace.workspace.uiSourceCodes().forEach(uiSourceCode=>uiSourceCode.removeAllLineDecorations(Components.CoverageProfile.LineDecorator.type));}};Components.CoverageProfile.LineDecorator=class{decorate(uiSourceCode,textEditor){var gutterType='CodeMirror-gutter-coverage';var decorations=uiSourceCode.lineDecorations(Components.CoverageProfile.LineDecorator.type);textEditor.uninstallGutter(gutterType);if(!decorations)
return;textEditor.installGutter(gutterType,false);for(var decoration of decorations.values()){var element=createElementWithClass('div','text-editor-line-marker-coverage');textEditor.setGutterDecoration(decoration.line(),gutterType,element);}}};Components.CoverageProfile.LineDecorator.type='coverage';;Runtime.cachedResources["components_lazy/filmStripDialog.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n:host {\n    flex: none !important;\n}\n\n.filmstrip-dialog {\n    margin: 12px;\n}\n\n.filmstrip-dialog > img {\n    border: 1px solid #ddd;\n    max-height: 80vh;\n    max-width: 80vw;\n}\n\n.filmstrip-dialog-footer {\n    display: flex;\n    align-items: center;\n    margin-top: 10px;\n}\n\n.filmstrip-dialog-label {\n    margin: 8px 8px;\n}\n\n/*# sourceURL=components_lazy/filmStripDialog.css */";Runtime.cachedResources["components_lazy/filmStripView.css"]="/*\n * Copyright (c) 2015 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.film-strip-view {\n    overflow-x: auto;\n    overflow-y: hidden;\n    align-content: flex-start;\n    min-height: 81px;\n}\n\n.film-strip-view.time-based .frame .time {\n    display: none;\n}\n\n.film-strip-view .label {\n    margin: auto;\n    font-size: 18px;\n    color: #999;\n}\n\n.film-strip-view .frame {\n    display: flex;\n    flex-direction: column;\n    align-items: center;\n    padding: 4px;\n    flex: none;\n    cursor: pointer;\n}\n\n.film-strip-view .frame-limit-reached {\n    font-size: 24px;\n    color: #888;\n    justify-content: center;\n    display: inline-flex;\n    flex-direction: column;\n    flex: none;\n}\n\n.film-strip-view .frame .thumbnail {\n    min-width: 24px;\n    display: flex;\n    flex-direction: row;\n    align-items: center;\n    pointer-events: none;\n    margin: 4px 0 2px;\n    border: 2px solid transparent;\n}\n\n.film-strip-view .frame:hover .thumbnail {\n    border-color: #FBCA46;\n}\n\n.film-strip-view .frame .thumbnail img {\n    height: auto;\n    width: auto;\n    max-width: 80px;\n    max-height: 50px;\n    pointer-events: none;\n    box-shadow: 0 0 3px #bbb;\n    flex: 0 0 auto;\n}\n\n.film-strip-view .frame:hover .thumbnail img {\n    box-shadow: none;\n}\n\n.film-strip-view .frame .time {\n    font-size: 10px;\n    margin-top: 2px;\n}\n\n/*# sourceURL=components_lazy/filmStripView.css */";