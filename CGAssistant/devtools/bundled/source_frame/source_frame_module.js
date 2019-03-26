SourceFrame.SourcesTextEditor=class extends TextEditor.CodeMirrorTextEditor{constructor(delegate){super({lineNumbers:true,lineWrapping:false,bracketMatchingSetting:Common.moduleSetting('textEditorBracketMatching'),});this.codeMirror().addKeyMap({'Enter':'smartNewlineAndIndent','Esc':'sourcesDismiss'});this._delegate=delegate;this.codeMirror().on('changes',this._fireTextChanged.bind(this));this.codeMirror().on('cursorActivity',this._cursorActivity.bind(this));this.codeMirror().on('gutterClick',this._gutterClick.bind(this));this.codeMirror().on('scroll',this._scroll.bind(this));this.codeMirror().on('focus',this._focus.bind(this));this.codeMirror().on('blur',this._blur.bind(this));this.codeMirror().on('beforeSelectionChange',this._fireBeforeSelectionChanged.bind(this));this.element.addEventListener('contextmenu',this._contextMenu.bind(this),false);this.codeMirror().addKeyMap(SourceFrame.SourcesTextEditor._BlockIndentController);this._tokenHighlighter=new SourceFrame.SourcesTextEditor.TokenHighlighter(this,this.codeMirror());this._gutters=['CodeMirror-linenumbers'];this.codeMirror().setOption('gutters',this._gutters.slice());this.codeMirror().setOption('electricChars',false);this.codeMirror().setOption('smartIndent',false);function updateAnticipateJumpFlag(value){this._isHandlingMouseDownEvent=value;}
this.element.addEventListener('mousedown',updateAnticipateJumpFlag.bind(this,true),true);this.element.addEventListener('mousedown',updateAnticipateJumpFlag.bind(this,false),false);Common.moduleSetting('textEditorIndent').addChangeListener(this._onUpdateEditorIndentation,this);Common.moduleSetting('textEditorAutoDetectIndent').addChangeListener(this._onUpdateEditorIndentation,this);Common.moduleSetting('showWhitespacesInEditor').addChangeListener(this._updateWhitespace,this);this._onUpdateEditorIndentation();this._setupWhitespaceHighlight();}
static _guessIndentationLevel(lines){var tabRegex=/^\t+/;var tabLines=0;var indents={};for(var lineNumber=0;lineNumber<lines.length;++lineNumber){var text=lines[lineNumber];if(text.length===0||!Common.TextUtils.isSpaceChar(text[0]))
continue;if(tabRegex.test(text)){++tabLines;continue;}
var i=0;while(i<text.length&&Common.TextUtils.isSpaceChar(text[i]))
++i;if(i%2!==0)
continue;indents[i]=1+(indents[i]||0);}
var linesCountPerIndentThreshold=3*lines.length/100;if(tabLines&&tabLines>linesCountPerIndentThreshold)
return'\t';var minimumIndent=Infinity;for(var i in indents){if(indents[i]<linesCountPerIndentThreshold)
continue;var indent=parseInt(i,10);if(minimumIndent>indent)
minimumIndent=indent;}
if(minimumIndent===Infinity)
return Common.moduleSetting('textEditorIndent').get();return' '.repeat(minimumIndent);}
_isSearchActive(){return!!this._tokenHighlighter.highlightedRegex();}
scrollToLine(lineNumber){super.scrollToLine(lineNumber);this._scroll();}
highlightSearchResults(regex,range){function innerHighlightRegex(){if(range){this.scrollLineIntoView(range.startLine);if(range.endColumn>TextEditor.CodeMirrorTextEditor.maxHighlightLength)
this.setSelection(range);else
this.setSelection(Common.TextRange.createFromLocation(range.startLine,range.startColumn));}
this._tokenHighlighter.highlightSearchResults(regex,range);}
if(!this._selectionBeforeSearch)
this._selectionBeforeSearch=this.selection();this.codeMirror().operation(innerHighlightRegex.bind(this));}
cancelSearchResultsHighlight(){this.codeMirror().operation(this._tokenHighlighter.highlightSelectedTokens.bind(this._tokenHighlighter));if(this._selectionBeforeSearch){this._reportJump(this._selectionBeforeSearch,this.selection());delete this._selectionBeforeSearch;}}
removeHighlight(highlightDescriptor){highlightDescriptor.clear();}
highlightRange(range,cssClass){cssClass='CodeMirror-persist-highlight '+cssClass;var pos=TextEditor.CodeMirrorUtils.toPos(range);++pos.end.ch;return this.codeMirror().markText(pos.start,pos.end,{className:cssClass,startStyle:cssClass+'-start',endStyle:cssClass+'-end'});}
addBreakpoint(lineNumber,disabled,conditional){if(lineNumber<0||lineNumber>=this.codeMirror().lineCount())
return;var className='cm-breakpoint'+(conditional?' cm-breakpoint-conditional':'')+
(disabled?' cm-breakpoint-disabled':'');this.codeMirror().addLineClass(lineNumber,'wrap',className);}
removeBreakpoint(lineNumber){if(lineNumber<0||lineNumber>=this.codeMirror().lineCount())
return;var wrapClasses=this.codeMirror().getLineHandle(lineNumber).wrapClass;if(!wrapClasses)
return;var classes=wrapClasses.split(' ');for(var i=0;i<classes.length;++i){if(classes[i].startsWith('cm-breakpoint'))
this.codeMirror().removeLineClass(lineNumber,'wrap',classes[i]);}}
installGutter(type,leftToNumbers){if(this._gutters.indexOf(type)!==-1)
return;if(leftToNumbers)
this._gutters.unshift(type);else
this._gutters.push(type);this.codeMirror().setOption('gutters',this._gutters.slice());this.refresh();}
uninstallGutter(type){var index=this._gutters.indexOf(type);if(index===-1)
return;this.codeMirror().clearGutter(type);this._gutters.splice(index,1);this.codeMirror().setOption('gutters',this._gutters.slice());this.refresh();}
setGutterDecoration(lineNumber,type,element){console.assert(this._gutters.indexOf(type)!==-1,'Cannot decorate unexisting gutter.');this.codeMirror().setGutterMarker(lineNumber,type,element);}
setExecutionLocation(lineNumber,columnNumber){this.clearPositionHighlight();this._executionLine=this.codeMirror().getLineHandle(lineNumber);if(!this._executionLine)
return;this.codeMirror().addLineClass(this._executionLine,'wrap','cm-execution-line');this._executionLineTailMarker=this.codeMirror().markText({line:lineNumber,ch:columnNumber},{line:lineNumber,ch:this.codeMirror().getLine(lineNumber).length},{className:'cm-execution-line-tail'});}
clearExecutionLine(){this.clearPositionHighlight();if(this._executionLine)
this.codeMirror().removeLineClass(this._executionLine,'wrap','cm-execution-line');delete this._executionLine;if(this._executionLineTailMarker)
this._executionLineTailMarker.clear();delete this._executionLineTailMarker;}
toggleLineClass(lineNumber,className,toggled){if(this.hasLineClass(lineNumber,className)===toggled)
return;var lineHandle=this.codeMirror().getLineHandle(lineNumber);if(!lineHandle)
return;if(toggled){this.codeMirror().addLineClass(lineHandle,'gutter',className);this.codeMirror().addLineClass(lineHandle,'wrap',className);}else{this.codeMirror().removeLineClass(lineHandle,'gutter',className);this.codeMirror().removeLineClass(lineHandle,'wrap',className);}}
hasLineClass(lineNumber,className){var lineInfo=this.codeMirror().lineInfo(lineNumber);var wrapClass=lineInfo.wrapClass||'';var classNames=wrapClass.split(' ');return classNames.indexOf(className)!==-1;}
_gutterClick(instance,lineNumber,gutter,event){this.dispatchEventToListeners(SourceFrame.SourcesTextEditor.Events.GutterClick,{lineNumber:lineNumber,event:event});}
_contextMenu(event){var contextMenu=new UI.ContextMenu(event);event.consume(true);var target=event.target.enclosingNodeOrSelfWithClass('CodeMirror-gutter-elt');var promise;if(target){promise=this._delegate.populateLineGutterContextMenu(contextMenu,parseInt(target.textContent,10)-1);}else{var textSelection=this.selection();promise=this._delegate.populateTextAreaContextMenu(contextMenu,textSelection.startLine,textSelection.startColumn);}
promise.then(showAsync.bind(this));function showAsync(){contextMenu.appendApplicableItems(this);contextMenu.show();}}
editRange(range,text,origin){var newRange=super.editRange(range,text,origin);this.dispatchEventToListeners(SourceFrame.SourcesTextEditor.Events.TextChanged,{oldRange:range,newRange:newRange});if(Common.moduleSetting('textEditorAutoDetectIndent').get())
this._onUpdateEditorIndentation();return newRange;}
_onUpdateEditorIndentation(){this._setEditorIndentation(TextEditor.CodeMirrorUtils.pullLines(this.codeMirror(),SourceFrame.SourcesTextEditor.LinesToScanForIndentationGuessing));}
_setEditorIndentation(lines){var extraKeys={};var indent=Common.moduleSetting('textEditorIndent').get();if(Common.moduleSetting('textEditorAutoDetectIndent').get())
indent=SourceFrame.SourcesTextEditor._guessIndentationLevel(lines);if(indent===Common.TextUtils.Indent.TabCharacter){this.codeMirror().setOption('indentWithTabs',true);this.codeMirror().setOption('indentUnit',4);}else{this.codeMirror().setOption('indentWithTabs',false);this.codeMirror().setOption('indentUnit',indent.length);extraKeys.Tab=function(codeMirror){if(codeMirror.somethingSelected())
return CodeMirror.Pass;var pos=codeMirror.getCursor('head');codeMirror.replaceRange(indent.substring(pos.ch%indent.length),codeMirror.getCursor());};}
this.codeMirror().setOption('extraKeys',extraKeys);this._indentationLevel=indent;}
indent(){return this._indentationLevel;}
_onAutoAppendedSpaces(){this._autoAppendedSpaces=this._autoAppendedSpaces||[];for(var i=0;i<this._autoAppendedSpaces.length;++i){var position=this._autoAppendedSpaces[i].resolve();if(!position)
continue;var line=this.line(position.lineNumber);if(line.length===position.columnNumber&&Common.TextUtils.lineIndent(line).length===line.length){this.codeMirror().replaceRange('',new CodeMirror.Pos(position.lineNumber,0),new CodeMirror.Pos(position.lineNumber,position.columnNumber));}}
this._autoAppendedSpaces=[];var selections=this.selections();for(var i=0;i<selections.length;++i){var selection=selections[i];this._autoAppendedSpaces.push(this.textEditorPositionHandle(selection.startLine,selection.startColumn));}}
_fireTextChanged(codeMirror,changes){if(!changes.length||this._muteTextChangedEvent)
return;var edits=[];var currentEdit;for(var changeIndex=0;changeIndex<changes.length;++changeIndex){var changeObject=changes[changeIndex];var edit=TextEditor.CodeMirrorUtils.changeObjectToEditOperation(changeObject);if(currentEdit&&edit.oldRange.equal(currentEdit.newRange)){currentEdit.newRange=edit.newRange;}else{currentEdit=edit;edits.push(currentEdit);}}
for(var i=0;i<edits.length;++i)
this.dispatchEventToListeners(SourceFrame.SourcesTextEditor.Events.TextChanged,edits[i]);}
_cursorActivity(){if(!this._isSearchActive())
this.codeMirror().operation(this._tokenHighlighter.highlightSelectedTokens.bind(this._tokenHighlighter));var start=this.codeMirror().getCursor('anchor');var end=this.codeMirror().getCursor('head');this.dispatchEventToListeners(SourceFrame.SourcesTextEditor.Events.SelectionChanged,TextEditor.CodeMirrorUtils.toRange(start,end));}
_reportJump(from,to){if(from&&to&&from.equal(to))
return;this.dispatchEventToListeners(SourceFrame.SourcesTextEditor.Events.JumpHappened,{from:from,to:to});}
_scroll(){var topmostLineNumber=this.codeMirror().lineAtHeight(this.codeMirror().getScrollInfo().top,'local');this.dispatchEventToListeners(SourceFrame.SourcesTextEditor.Events.ScrollChanged,topmostLineNumber);}
_focus(){this.dispatchEventToListeners(SourceFrame.SourcesTextEditor.Events.EditorFocused);}
_blur(){this.dispatchEventToListeners(SourceFrame.SourcesTextEditor.Events.EditorBlurred);}
_fireBeforeSelectionChanged(codeMirror,selection){if(!this._isHandlingMouseDownEvent)
return;if(!selection.ranges.length)
return;var primarySelection=selection.ranges[0];this._reportJump(this.selection(),TextEditor.CodeMirrorUtils.toRange(primarySelection.anchor,primarySelection.head));}
dispose(){super.dispose();Common.moduleSetting('textEditorIndent').removeChangeListener(this._onUpdateEditorIndentation,this);Common.moduleSetting('textEditorAutoDetectIndent').removeChangeListener(this._onUpdateEditorIndentation,this);Common.moduleSetting('showWhitespacesInEditor').removeChangeListener(this._updateWhitespace,this);}
setText(text){this._muteTextChangedEvent=true;this._setEditorIndentation(text.split('\n').slice(0,SourceFrame.SourcesTextEditor.LinesToScanForIndentationGuessing));super.setText(text);delete this._muteTextChangedEvent;}
_updateWhitespace(){this.setMimeType(this.mimeType());}
rewriteMimeType(mimeType){this._setupWhitespaceHighlight();var whitespaceMode=Common.moduleSetting('showWhitespacesInEditor').get();this.element.classList.toggle('show-whitespaces',whitespaceMode==='all');if(whitespaceMode==='all')
return this._allWhitespaceOverlayMode(mimeType);else if(whitespaceMode==='trailing')
return this._trailingWhitespaceOverlayMode(mimeType);return mimeType;}
_allWhitespaceOverlayMode(mimeType){var modeName=CodeMirror.mimeModes[mimeType]?(CodeMirror.mimeModes[mimeType].name||CodeMirror.mimeModes[mimeType]):CodeMirror.mimeModes['text/plain'];modeName+='+all-whitespaces';if(CodeMirror.modes[modeName])
return modeName;function modeConstructor(config,parserConfig){function nextToken(stream){if(stream.peek()===' '){var spaces=0;while(spaces<SourceFrame.SourcesTextEditor.MaximumNumberOfWhitespacesPerSingleSpan&&stream.peek()===' '){++spaces;stream.next();}
return'whitespace whitespace-'+spaces;}
while(!stream.eol()&&stream.peek()!==' ')
stream.next();return null;}
var whitespaceMode={token:nextToken};return CodeMirror.overlayMode(CodeMirror.getMode(config,mimeType),whitespaceMode,false);}
CodeMirror.defineMode(modeName,modeConstructor);return modeName;}
_trailingWhitespaceOverlayMode(mimeType){var modeName=CodeMirror.mimeModes[mimeType]?(CodeMirror.mimeModes[mimeType].name||CodeMirror.mimeModes[mimeType]):CodeMirror.mimeModes['text/plain'];modeName+='+trailing-whitespaces';if(CodeMirror.modes[modeName])
return modeName;function modeConstructor(config,parserConfig){function nextToken(stream){var pos=stream.pos;if(stream.match(/^\s+$/,true))
return true?'trailing-whitespace':null;do
stream.next();while(!stream.eol()&&stream.peek()!==' ');return null;}
var whitespaceMode={token:nextToken};return CodeMirror.overlayMode(CodeMirror.getMode(config,mimeType),whitespaceMode,false);}
CodeMirror.defineMode(modeName,modeConstructor);return modeName;}
_setupWhitespaceHighlight(){var doc=this.element.ownerDocument;if(doc._codeMirrorWhitespaceStyleInjected||!Common.moduleSetting('showWhitespacesInEditor').get())
return;doc._codeMirrorWhitespaceStyleInjected=true;const classBase='.show-whitespaces .CodeMirror .cm-whitespace-';const spaceChar='·';var spaceChars='';var rules='';for(var i=1;i<=SourceFrame.SourcesTextEditor.MaximumNumberOfWhitespacesPerSingleSpan;++i){spaceChars+=spaceChar;var rule=classBase+i+'::before { content: \''+spaceChars+'\';}\n';rules+=rule;}
var style=doc.createElement('style');style.textContent=rules;doc.head.appendChild(style);}};SourceFrame.SourcesTextEditor.GutterClickEventData;SourceFrame.SourcesTextEditor.Events={GutterClick:Symbol('GutterClick'),TextChanged:Symbol('TextChanged'),SelectionChanged:Symbol('SelectionChanged'),ScrollChanged:Symbol('ScrollChanged'),EditorFocused:Symbol('EditorFocused'),EditorBlurred:Symbol('EditorBlurred'),JumpHappened:Symbol('JumpHappened')};SourceFrame.SourcesTextEditorDelegate=function(){};SourceFrame.SourcesTextEditorDelegate.prototype={populateLineGutterContextMenu:function(contextMenu,lineNumber){},populateTextAreaContextMenu:function(contextMenu,lineNumber,columnNumber){},};CodeMirror.commands.smartNewlineAndIndent=function(codeMirror){codeMirror.operation(innerSmartNewlineAndIndent.bind(null,codeMirror));function innerSmartNewlineAndIndent(codeMirror){var selections=codeMirror.listSelections();var replacements=[];for(var i=0;i<selections.length;++i){var selection=selections[i];var cur=CodeMirror.cmpPos(selection.head,selection.anchor)<0?selection.head:selection.anchor;var line=codeMirror.getLine(cur.line);var indent=Common.TextUtils.lineIndent(line);replacements.push('\n'+indent.substring(0,Math.min(cur.ch,indent.length)));}
codeMirror.replaceSelections(replacements);codeMirror._codeMirrorTextEditor._onAutoAppendedSpaces();}};CodeMirror.commands.sourcesDismiss=function(codemirror){if(codemirror.listSelections().length===1&&codemirror._codeMirrorTextEditor._isSearchActive())
return CodeMirror.Pass;return CodeMirror.commands.dismiss(codemirror);};SourceFrame.SourcesTextEditor._BlockIndentController={name:'blockIndentKeymap',Enter:function(codeMirror){var selections=codeMirror.listSelections();var replacements=[];var allSelectionsAreCollapsedBlocks=false;for(var i=0;i<selections.length;++i){var selection=selections[i];var start=CodeMirror.cmpPos(selection.head,selection.anchor)<0?selection.head:selection.anchor;var line=codeMirror.getLine(start.line);var indent=Common.TextUtils.lineIndent(line);var indentToInsert='\n'+indent+codeMirror._codeMirrorTextEditor.indent();var isCollapsedBlock=false;if(selection.head.ch===0)
return CodeMirror.Pass;if(line.substr(selection.head.ch-1,2)==='{}'){indentToInsert+='\n'+indent;isCollapsedBlock=true;}else if(line.substr(selection.head.ch-1,1)!=='{'){return CodeMirror.Pass;}
if(i>0&&allSelectionsAreCollapsedBlocks!==isCollapsedBlock)
return CodeMirror.Pass;replacements.push(indentToInsert);allSelectionsAreCollapsedBlocks=isCollapsedBlock;}
codeMirror.replaceSelections(replacements);if(!allSelectionsAreCollapsedBlocks){codeMirror._codeMirrorTextEditor._onAutoAppendedSpaces();return;}
selections=codeMirror.listSelections();var updatedSelections=[];for(var i=0;i<selections.length;++i){var selection=selections[i];var line=codeMirror.getLine(selection.head.line-1);var position=new CodeMirror.Pos(selection.head.line-1,line.length);updatedSelections.push({head:position,anchor:position});}
codeMirror.setSelections(updatedSelections);codeMirror._codeMirrorTextEditor._onAutoAppendedSpaces();},'\'}\'':function(codeMirror){if(codeMirror.somethingSelected())
return CodeMirror.Pass;var selections=codeMirror.listSelections();var replacements=[];for(var i=0;i<selections.length;++i){var selection=selections[i];var line=codeMirror.getLine(selection.head.line);if(line!==Common.TextUtils.lineIndent(line))
return CodeMirror.Pass;replacements.push('}');}
codeMirror.replaceSelections(replacements);selections=codeMirror.listSelections();replacements=[];var updatedSelections=[];for(var i=0;i<selections.length;++i){var selection=selections[i];var matchingBracket=codeMirror.findMatchingBracket(selection.head);if(!matchingBracket||!matchingBracket.match)
return;updatedSelections.push({head:selection.head,anchor:new CodeMirror.Pos(selection.head.line,0)});var line=codeMirror.getLine(matchingBracket.to.line);var indent=Common.TextUtils.lineIndent(line);replacements.push(indent+'}');}
codeMirror.setSelections(updatedSelections);codeMirror.replaceSelections(replacements);}};SourceFrame.SourcesTextEditor.TokenHighlighter=class{constructor(textEditor,codeMirror){this._textEditor=textEditor;this._codeMirror=codeMirror;}
highlightSearchResults(regex,range){var oldRegex=this._highlightRegex;this._highlightRegex=regex;this._highlightRange=range;if(this._searchResultMarker){this._searchResultMarker.clear();delete this._searchResultMarker;}
if(this._highlightDescriptor&&this._highlightDescriptor.selectionStart)
this._codeMirror.removeLineClass(this._highlightDescriptor.selectionStart.line,'wrap','cm-line-with-selection');var selectionStart=this._highlightRange?new CodeMirror.Pos(this._highlightRange.startLine,this._highlightRange.startColumn):null;if(selectionStart)
this._codeMirror.addLineClass(selectionStart.line,'wrap','cm-line-with-selection');if(oldRegex&&this._highlightRegex.toString()===oldRegex.toString()){if(this._highlightDescriptor)
this._highlightDescriptor.selectionStart=selectionStart;}else{this._removeHighlight();this._setHighlighter(this._searchHighlighter.bind(this,this._highlightRegex),selectionStart);}
if(this._highlightRange){var pos=TextEditor.CodeMirrorUtils.toPos(this._highlightRange);this._searchResultMarker=this._codeMirror.markText(pos.start,pos.end,{className:'cm-column-with-selection'});}}
highlightedRegex(){return this._highlightRegex;}
highlightSelectedTokens(){delete this._highlightRegex;delete this._highlightRange;if(this._highlightDescriptor&&this._highlightDescriptor.selectionStart)
this._codeMirror.removeLineClass(this._highlightDescriptor.selectionStart.line,'wrap','cm-line-with-selection');this._removeHighlight();var selectionStart=this._codeMirror.getCursor('start');var selectionEnd=this._codeMirror.getCursor('end');if(selectionStart.line!==selectionEnd.line)
return;if(selectionStart.ch===selectionEnd.ch)
return;var selections=this._codeMirror.getSelections();if(selections.length>1)
return;var selectedText=selections[0];if(this._isWord(selectedText,selectionStart.line,selectionStart.ch,selectionEnd.ch)){if(selectionStart)
this._codeMirror.addLineClass(selectionStart.line,'wrap','cm-line-with-selection');this._setHighlighter(this._tokenHighlighter.bind(this,selectedText,selectionStart),selectionStart);}}
_isWord(selectedText,lineNumber,startColumn,endColumn){var line=this._codeMirror.getLine(lineNumber);var leftBound=startColumn===0||!Common.TextUtils.isWordChar(line.charAt(startColumn-1));var rightBound=endColumn===line.length||!Common.TextUtils.isWordChar(line.charAt(endColumn));return leftBound&&rightBound&&Common.TextUtils.isWord(selectedText);}
_removeHighlight(){if(this._highlightDescriptor){this._codeMirror.removeOverlay(this._highlightDescriptor.overlay);delete this._highlightDescriptor;}}
_searchHighlighter(regex,stream){if(stream.column()===0)
delete this._searchMatchLength;if(this._searchMatchLength){if(this._searchMatchLength>2){for(var i=0;i<this._searchMatchLength-2;++i)
stream.next();this._searchMatchLength=1;return'search-highlight';}else{stream.next();delete this._searchMatchLength;return'search-highlight search-highlight-end';}}
var match=stream.match(regex,false);if(match){stream.next();var matchLength=match[0].length;if(matchLength===1)
return'search-highlight search-highlight-full';this._searchMatchLength=matchLength;return'search-highlight search-highlight-start';}
while(!stream.match(regex,false)&&stream.next()){}}
_tokenHighlighter(token,selectionStart,stream){var tokenFirstChar=token.charAt(0);if(stream.match(token)&&(stream.eol()||!Common.TextUtils.isWordChar(stream.peek())))
return stream.column()===selectionStart.ch?'token-highlight column-with-selection':'token-highlight';var eatenChar;do
eatenChar=stream.next();while(eatenChar&&(Common.TextUtils.isWordChar(eatenChar)||stream.peek()!==tokenFirstChar));}
_setHighlighter(highlighter,selectionStart){var overlayMode={token:highlighter};this._codeMirror.addOverlay(overlayMode);this._highlightDescriptor={overlay:overlayMode,selectionStart:selectionStart};}};SourceFrame.SourcesTextEditor.LinesToScanForIndentationGuessing=1000;SourceFrame.SourcesTextEditor.MaximumNumberOfWhitespacesPerSingleSpan=16;;SourceFrame.FontView=class extends UI.SimpleView{constructor(mimeType,contentProvider){super(Common.UIString('Font'));this.registerRequiredCSS('source_frame/fontView.css');this.element.classList.add('font-view');this._url=contentProvider.contentURL();this._mimeType=mimeType;this._contentProvider=contentProvider;this._mimeTypeLabel=new UI.ToolbarText(mimeType);}
syncToolbarItems(){return[this._mimeTypeLabel];}
_onFontContentLoaded(uniqueFontName,content){var url=content?Common.ContentProvider.contentAsDataURL(content,this._mimeType,true):this._url;this.fontStyleElement.textContent=String.sprintf('@font-face { font-family: "%s"; src: url(%s); }',uniqueFontName,url);}
_createContentIfNeeded(){if(this.fontPreviewElement)
return;var uniqueFontName='WebInspectorFontPreview'+(++SourceFrame.FontView._fontId);this.fontStyleElement=createElement('style');this._contentProvider.requestContent().then(this._onFontContentLoaded.bind(this,uniqueFontName));this.element.appendChild(this.fontStyleElement);var fontPreview=createElement('div');for(var i=0;i<SourceFrame.FontView._fontPreviewLines.length;++i){if(i>0)
fontPreview.createChild('br');fontPreview.createTextChild(SourceFrame.FontView._fontPreviewLines[i]);}
this.fontPreviewElement=fontPreview.cloneNode(true);this.fontPreviewElement.style.setProperty('font-family',uniqueFontName);this.fontPreviewElement.style.setProperty('visibility','hidden');this._dummyElement=fontPreview;this._dummyElement.style.visibility='hidden';this._dummyElement.style.zIndex='-1';this._dummyElement.style.display='inline';this._dummyElement.style.position='absolute';this._dummyElement.style.setProperty('font-family',uniqueFontName);this._dummyElement.style.setProperty('font-size',SourceFrame.FontView._measureFontSize+'px');this.element.appendChild(this.fontPreviewElement);}
wasShown(){this._createContentIfNeeded();this.updateFontPreviewSize();}
onResize(){if(this._inResize)
return;this._inResize=true;try{this.updateFontPreviewSize();}finally{delete this._inResize;}}
_measureElement(){this.element.appendChild(this._dummyElement);var result={width:this._dummyElement.offsetWidth,height:this._dummyElement.offsetHeight};this.element.removeChild(this._dummyElement);return result;}
updateFontPreviewSize(){if(!this.fontPreviewElement||!this.isShowing())
return;this.fontPreviewElement.style.removeProperty('visibility');var dimension=this._measureElement();const height=dimension.height;const width=dimension.width;const containerWidth=this.element.offsetWidth-50;const containerHeight=this.element.offsetHeight-30;if(!height||!width||!containerWidth||!containerHeight){this.fontPreviewElement.style.removeProperty('font-size');return;}
var widthRatio=containerWidth/width;var heightRatio=containerHeight/height;var finalFontSize=Math.floor(SourceFrame.FontView._measureFontSize*Math.min(widthRatio,heightRatio))-2;this.fontPreviewElement.style.setProperty('font-size',finalFontSize+'px',null);}};SourceFrame.FontView._fontPreviewLines=['ABCDEFGHIJKLM','NOPQRSTUVWXYZ','abcdefghijklm','nopqrstuvwxyz','1234567890'];SourceFrame.FontView._fontId=0;SourceFrame.FontView._measureFontSize=50;;SourceFrame.ImageView=class extends UI.SimpleView{constructor(mimeType,contentProvider){super(Common.UIString('Image'));this.registerRequiredCSS('source_frame/imageView.css');this.element.classList.add('image-view');this._url=contentProvider.contentURL();this._parsedURL=new Common.ParsedURL(this._url);this._mimeType=mimeType;this._contentProvider=contentProvider;this._sizeLabel=new UI.ToolbarText();this._dimensionsLabel=new UI.ToolbarText();this._mimeTypeLabel=new UI.ToolbarText(mimeType);}
syncToolbarItems(){return[this._sizeLabel,new UI.ToolbarSeparator(),this._dimensionsLabel,new UI.ToolbarSeparator(),this._mimeTypeLabel];}
wasShown(){this._createContentIfNeeded();}
_createContentIfNeeded(){if(this._container)
return;this._container=this.element.createChild('div','image');var imagePreviewElement=this._container.createChild('img','resource-image-view');imagePreviewElement.addEventListener('contextmenu',this._contextMenu.bind(this),true);this._contentProvider.requestContent().then(onContentAvailable.bind(this));function onContentAvailable(content){var imageSrc=Common.ContentProvider.contentAsDataURL(content,this._mimeType,true);if(imageSrc===null)
imageSrc=this._url;imagePreviewElement.src=imageSrc;this._sizeLabel.setText(Number.bytesToString(this._base64ToSize(content)));this._dimensionsLabel.setText(Common.UIString('%d × %d',imagePreviewElement.naturalWidth,imagePreviewElement.naturalHeight));}
this._imagePreviewElement=imagePreviewElement;}
_base64ToSize(content){if(!content||!content.length)
return 0;var size=(content.length||0)*3/4;if(content.length>0&&content[content.length-1]==='=')
size--;if(content.length>1&&content[content.length-2]==='=')
size--;return size;}
_contextMenu(event){var contextMenu=new UI.ContextMenu(event);if(!this._parsedURL.isDataURL())
contextMenu.appendItem(Common.UIString.capitalize('Copy ^image URL'),this._copyImageURL.bind(this));if(this._imagePreviewElement.src){contextMenu.appendItem(Common.UIString.capitalize('Copy ^image as Data URI'),this._copyImageAsDataURL.bind(this));}
contextMenu.appendItem(Common.UIString.capitalize('Open ^image in ^new ^tab'),this._openInNewTab.bind(this));contextMenu.appendItem(Common.UIString.capitalize('Save\u2026'),this._saveImage.bind(this));contextMenu.show();}
_copyImageAsDataURL(){InspectorFrontendHost.copyText(this._imagePreviewElement.src);}
_copyImageURL(){InspectorFrontendHost.copyText(this._url);}
_saveImage(){var link=createElement('a');link.download=this._parsedURL.displayName;link.href=this._url;link.click();}
_openInNewTab(){InspectorFrontendHost.openInNewTab(this._url);}};;SourceFrame.SourceFrame=class extends UI.SimpleView{constructor(url,lazyContent){super(Common.UIString('Source'));this._url=url;this._lazyContent=lazyContent;this._textEditor=new SourceFrame.SourcesTextEditor(this);this._currentSearchResultIndex=-1;this._searchResults=[];this._textEditor.addEventListener(SourceFrame.SourcesTextEditor.Events.EditorFocused,this._resetCurrentSearchResultIndex,this);this._textEditor.addEventListener(SourceFrame.SourcesTextEditor.Events.SelectionChanged,this._updateSourcePosition,this);this._textEditor.addEventListener(SourceFrame.SourcesTextEditor.Events.TextChanged,event=>this.onTextChanged(event.data.oldRange,event.data.newRange));this._shortcuts={};this.element.addEventListener('keydown',this._handleKeyDown.bind(this),false);this._sourcePosition=new UI.ToolbarText();this._searchableView=null;this._editable=false;this._textEditor.setReadOnly(true);}
setEditable(editable){this._editable=editable;this._textEditor.setReadOnly(!editable);}
addShortcut(key,handler){this._shortcuts[key]=handler;}
wasShown(){this._ensureContentLoaded();this._textEditor.show(this.element);this._editorAttached=true;this._wasShownOrLoaded();}
isEditorShowing(){return this.isShowing()&&this._editorAttached;}
willHide(){super.willHide();this._clearPositionToReveal();}
syncToolbarItems(){return[this._sourcePosition];}
get loaded(){return this._loaded;}
get textEditor(){return this._textEditor;}
_ensureContentLoaded(){if(!this._contentRequested){this._contentRequested=true;this._lazyContent().then(this.setContent.bind(this));}}
revealPosition(line,column,shouldHighlight){this._clearLineToScrollTo();this._clearSelectionToSet();this._positionToReveal={line:line,column:column,shouldHighlight:shouldHighlight};this._innerRevealPositionIfNeeded();}
_innerRevealPositionIfNeeded(){if(!this._positionToReveal)
return;if(!this.loaded||!this.isEditorShowing())
return;this._textEditor.revealPosition(this._positionToReveal.line,this._positionToReveal.column,this._positionToReveal.shouldHighlight);delete this._positionToReveal;}
_clearPositionToReveal(){this._textEditor.clearPositionHighlight();delete this._positionToReveal;}
scrollToLine(line){this._clearPositionToReveal();this._lineToScrollTo=line;this._innerScrollToLineIfNeeded();}
_innerScrollToLineIfNeeded(){if(typeof this._lineToScrollTo==='number'){if(this.loaded&&this.isEditorShowing()){this._textEditor.scrollToLine(this._lineToScrollTo);delete this._lineToScrollTo;}}}
_clearLineToScrollTo(){delete this._lineToScrollTo;}
selection(){return this.textEditor.selection();}
setSelection(textRange){this._selectionToSet=textRange;this._innerSetSelectionIfNeeded();}
_innerSetSelectionIfNeeded(){if(this._selectionToSet&&this.loaded&&this.isEditorShowing()){this._textEditor.setSelection(this._selectionToSet);delete this._selectionToSet;}}
_clearSelectionToSet(){delete this._selectionToSet;}
_wasShownOrLoaded(){this._innerRevealPositionIfNeeded();this._innerSetSelectionIfNeeded();this._innerScrollToLineIfNeeded();}
onTextChanged(oldRange,newRange){if(this._searchConfig&&this._searchableView)
this.performSearch(this._searchConfig,false,false);}
_simplifyMimeType(content,mimeType){if(!mimeType)
return'';if(mimeType.indexOf('javascript')>=0||mimeType.indexOf('jscript')>=0||mimeType.indexOf('ecmascript')>=0)
return'text/javascript';if(mimeType==='text/x-php'&&content.match(/\<\?.*\?\>/g))
return'application/x-httpd-php';return mimeType;}
setHighlighterType(highlighterType){this._highlighterType=highlighterType;this._updateHighlighterType('');}
_updateHighlighterType(content){this._textEditor.setMimeType(this._simplifyMimeType(content,this._highlighterType));}
setContent(content){if(!this._loaded){this._loaded=true;this._textEditor.setText(content||'');this._textEditor.markClean();}else{var scrollTop=this._textEditor.scrollTop();var selection=this._textEditor.selection();this._textEditor.setText(content||'');this._textEditor.setScrollTop(scrollTop);this._textEditor.setSelection(selection);}
this._updateHighlighterType(content||'');this._wasShownOrLoaded();if(this._delayedFindSearchMatches){this._delayedFindSearchMatches();delete this._delayedFindSearchMatches;}
this.onTextEditorContentSet();}
onTextEditorContentSet(){}
setSearchableView(view){this._searchableView=view;}
_doFindSearchMatches(searchConfig,shouldJump,jumpBackwards){this._currentSearchResultIndex=-1;this._searchResults=[];var regex=searchConfig.toSearchRegex();this._searchRegex=regex;this._searchResults=this._collectRegexMatches(regex);if(this._searchableView)
this._searchableView.updateSearchMatchesCount(this._searchResults.length);if(!this._searchResults.length)
this._textEditor.cancelSearchResultsHighlight();else if(shouldJump&&jumpBackwards)
this.jumpToPreviousSearchResult();else if(shouldJump)
this.jumpToNextSearchResult();else
this._textEditor.highlightSearchResults(regex,null);}
performSearch(searchConfig,shouldJump,jumpBackwards){if(this._searchableView)
this._searchableView.updateSearchMatchesCount(0);this._resetSearch();this._searchConfig=searchConfig;if(this.loaded)
this._doFindSearchMatches(searchConfig,shouldJump,!!jumpBackwards);else
this._delayedFindSearchMatches=this._doFindSearchMatches.bind(this,searchConfig,shouldJump,!!jumpBackwards);this._ensureContentLoaded();}
_resetCurrentSearchResultIndex(){if(!this._searchResults.length)
return;this._currentSearchResultIndex=-1;if(this._searchableView)
this._searchableView.updateCurrentMatchIndex(this._currentSearchResultIndex);this._textEditor.highlightSearchResults(this._searchRegex,null);}
_resetSearch(){delete this._searchConfig;delete this._delayedFindSearchMatches;this._currentSearchResultIndex=-1;this._searchResults=[];delete this._searchRegex;}
searchCanceled(){var range=this._currentSearchResultIndex!==-1?this._searchResults[this._currentSearchResultIndex]:null;this._resetSearch();if(!this.loaded)
return;this._textEditor.cancelSearchResultsHighlight();if(range)
this.setSelection(range);}
hasSearchResults(){return this._searchResults.length>0;}
jumpToFirstSearchResult(){this.jumpToSearchResult(0);}
jumpToLastSearchResult(){this.jumpToSearchResult(this._searchResults.length-1);}
_searchResultIndexForCurrentSelection(){return this._searchResults.lowerBound(this._textEditor.selection().collapseToEnd(),Common.TextRange.comparator);}
jumpToNextSearchResult(){var currentIndex=this._searchResultIndexForCurrentSelection();var nextIndex=this._currentSearchResultIndex===-1?currentIndex:currentIndex+1;this.jumpToSearchResult(nextIndex);}
jumpToPreviousSearchResult(){var currentIndex=this._searchResultIndexForCurrentSelection();this.jumpToSearchResult(currentIndex-1);}
supportsCaseSensitiveSearch(){return true;}
supportsRegexSearch(){return true;}
get currentSearchResultIndex(){return this._currentSearchResultIndex;}
jumpToSearchResult(index){if(!this.loaded||!this._searchResults.length)
return;this._currentSearchResultIndex=(index+this._searchResults.length)%this._searchResults.length;if(this._searchableView)
this._searchableView.updateCurrentMatchIndex(this._currentSearchResultIndex);this._textEditor.highlightSearchResults(this._searchRegex,this._searchResults[this._currentSearchResultIndex]);}
replaceSelectionWith(searchConfig,replacement){var range=this._searchResults[this._currentSearchResultIndex];if(!range)
return;this._textEditor.highlightSearchResults(this._searchRegex,null);var oldText=this._textEditor.text(range);var regex=searchConfig.toSearchRegex();var text;if(regex.__fromRegExpQuery){text=oldText.replace(regex,replacement);}else{text=oldText.replace(regex,function(){return replacement;});}
var newRange=this._textEditor.editRange(range,text);this._textEditor.setSelection(newRange.collapseToEnd());}
replaceAllWith(searchConfig,replacement){this._resetCurrentSearchResultIndex();var text=this._textEditor.text();var range=this._textEditor.fullRange();var regex=searchConfig.toSearchRegex(true);if(regex.__fromRegExpQuery){text=text.replace(regex,replacement);}else{text=text.replace(regex,function(){return replacement;});}
var ranges=this._collectRegexMatches(regex);if(!ranges.length)
return;var currentRangeIndex=ranges.lowerBound(this._textEditor.selection(),Common.TextRange.comparator);var lastRangeIndex=mod(currentRangeIndex-1,ranges.length);var lastRange=ranges[lastRangeIndex];var replacementLineEndings=replacement.computeLineEndings();var replacementLineCount=replacementLineEndings.length;var lastLineNumber=lastRange.startLine+replacementLineEndings.length-1;var lastColumnNumber=lastRange.startColumn;if(replacementLineEndings.length>1){lastColumnNumber=replacementLineEndings[replacementLineCount-1]-replacementLineEndings[replacementLineCount-2]-1;}
this._textEditor.editRange(range,text);this._textEditor.revealPosition(lastLineNumber,lastColumnNumber);this._textEditor.setSelection(Common.TextRange.createFromLocation(lastLineNumber,lastColumnNumber));}
_collectRegexMatches(regexObject){var ranges=[];for(var i=0;i<this._textEditor.linesCount;++i){var line=this._textEditor.line(i);var offset=0;do{var match=regexObject.exec(line);if(match){var matchEndIndex=match.index+Math.max(match[0].length,1);if(match[0].length)
ranges.push(new Common.TextRange(i,offset+match.index,i,offset+matchEndIndex));offset+=matchEndIndex;line=line.substring(matchEndIndex);}}while(match&&line);}
return ranges;}
populateLineGutterContextMenu(contextMenu,lineNumber){return Promise.resolve();}
populateTextAreaContextMenu(contextMenu,lineNumber,columnNumber){return Promise.resolve();}
canEditSource(){return this._editable;}
_updateSourcePosition(){var selections=this._textEditor.selections();if(!selections.length)
return;if(selections.length>1){this._sourcePosition.setText(Common.UIString('%d selection regions',selections.length));return;}
var textRange=selections[0];if(textRange.isEmpty()){this._sourcePosition.setText(Common.UIString('Line %d, Column %d',textRange.endLine+1,textRange.endColumn+1));return;}
textRange=textRange.normalize();var selectedText=this._textEditor.text(textRange);if(textRange.startLine===textRange.endLine){this._sourcePosition.setText(Common.UIString('%d characters selected',selectedText.length));}else{this._sourcePosition.setText(Common.UIString('%d lines, %d characters selected',textRange.endLine-textRange.startLine+1,selectedText.length));}}
_handleKeyDown(e){var shortcutKey=UI.KeyboardShortcut.makeKeyFromEvent(e);var handler=this._shortcuts[shortcutKey];if(handler&&handler())
e.consume(true);}};;SourceFrame.ResourceSourceFrame=class extends SourceFrame.SourceFrame{constructor(resource){super(resource.contentURL(),resource.requestContent.bind(resource));this._resource=resource;}
static createSearchableView(resource,highlighterType){var sourceFrame=new SourceFrame.ResourceSourceFrame(resource);sourceFrame.setHighlighterType(highlighterType);var searchableView=new UI.SearchableView(sourceFrame);searchableView.setPlaceholder(Common.UIString('Find'));sourceFrame.show(searchableView.element);sourceFrame.setSearchableView(searchableView);return searchableView;}
get resource(){return this._resource;}
populateTextAreaContextMenu(contextMenu,lineNumber,columnNumber){contextMenu.appendApplicableItems(this._resource);return Promise.resolve();}};;Runtime.cachedResources["source_frame/fontView.css"]="/*\n * Copyright (c) 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.font-view {\n    font-size: 60px;\n    white-space: pre-wrap;\n    word-wrap: break-word;\n    text-align: center;\n    padding: 15px;\n}\n\n/*# sourceURL=source_frame/fontView.css */";Runtime.cachedResources["source_frame/imageView.css"]="/*\n * Copyright (c) 2014 The Chromium Authors. All rights reserved.\n * Use of this source code is governed by a BSD-style license that can be\n * found in the LICENSE file.\n */\n\n.image-view {\n    overflow: auto;\n}\n\n.image-view > .image {\n    padding: 20px 20px 10px 20px;\n    text-align: center;\n}\n\n.image-view img.resource-image-view {\n    max-width: 100%;\n    max-height: 1000px;\n    background-image: url(Images/checker.png);\n    box-shadow: 0 5px 10px rgba(0, 0, 0, 0.5);\n    -webkit-user-select: text;\n    -webkit-user-drag: auto;\n}\n\n/*# sourceURL=source_frame/imageView.css */";