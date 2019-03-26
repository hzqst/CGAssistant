Console.ConsoleContextSelector=class{constructor(selectElement){this._selectElement=selectElement;this._optionByExecutionContext=new Map();SDK.targetManager.observeTargets(this);SDK.targetManager.addModelListener(SDK.RuntimeModel,SDK.RuntimeModel.Events.ExecutionContextCreated,this._onExecutionContextCreated,this);SDK.targetManager.addModelListener(SDK.RuntimeModel,SDK.RuntimeModel.Events.ExecutionContextChanged,this._onExecutionContextChanged,this);SDK.targetManager.addModelListener(SDK.RuntimeModel,SDK.RuntimeModel.Events.ExecutionContextDestroyed,this._onExecutionContextDestroyed,this);this._selectElement.addEventListener('change',this._executionContextChanged.bind(this),false);UI.context.addFlavorChangeListener(SDK.ExecutionContext,this._executionContextChangedExternally,this);}
_titleFor(executionContext){var result;if(executionContext.isDefault){if(executionContext.frameId){var resourceTreeModel=SDK.ResourceTreeModel.fromTarget(executionContext.target());var frame=resourceTreeModel&&resourceTreeModel.frameForId(executionContext.frameId);result=frame?frame.displayName():executionContext.label();}else{result=executionContext.target().decorateLabel(executionContext.label());}}else{result='\u00a0\u00a0\u00a0\u00a0'+(executionContext.label()||executionContext.origin);}
var maxLength=50;return result.trimMiddle(maxLength);}
_executionContextCreated(executionContext){if(!executionContext.target().hasJSCapability())
return;var newOption=createElement('option');newOption.__executionContext=executionContext;newOption.text=this._titleFor(executionContext);this._optionByExecutionContext.set(executionContext,newOption);var options=this._selectElement.options;var contexts=Array.prototype.map.call(options,mapping);var index=contexts.lowerBound(executionContext,executionContext.runtimeModel.executionContextComparator());this._selectElement.insertBefore(newOption,options[index]);if(executionContext===UI.context.flavor(SDK.ExecutionContext))
this._select(newOption);function mapping(option){return option.__executionContext;}}
_onExecutionContextCreated(event){var executionContext=(event.data);this._executionContextCreated(executionContext);this._updateSelectionWarning();}
_onExecutionContextChanged(event){var executionContext=(event.data);var option=this._optionByExecutionContext.get(executionContext);if(option)
option.text=this._titleFor(executionContext);this._updateSelectionWarning();}
_executionContextDestroyed(executionContext){var option=this._optionByExecutionContext.remove(executionContext);option.remove();}
_onExecutionContextDestroyed(event){var executionContext=(event.data);this._executionContextDestroyed(executionContext);this._updateSelectionWarning();}
_executionContextChangedExternally(event){var executionContext=(event.data);if(!executionContext)
return;var options=this._selectElement.options;for(var i=0;i<options.length;++i){if(options[i].__executionContext===executionContext)
this._select(options[i]);}}
_executionContextChanged(){var option=this._selectedOption();var newContext=option?option.__executionContext:null;UI.context.setFlavor(SDK.ExecutionContext,newContext);this._updateSelectionWarning();}
_updateSelectionWarning(){var executionContext=UI.context.flavor(SDK.ExecutionContext);this._selectElement.parentElement.classList.toggle('warning',!this._isTopContext(executionContext)&&this._hasTopContext());}
_isTopContext(executionContext){if(!executionContext||!executionContext.isDefault)
return false;var resourceTreeModel=SDK.ResourceTreeModel.fromTarget(executionContext.target());var frame=executionContext.frameId&&resourceTreeModel&&resourceTreeModel.frameForId(executionContext.frameId);if(!frame)
return false;return frame.isMainFrame();}
_hasTopContext(){var options=this._selectElement.options;for(var i=0;i<options.length;i++){if(this._isTopContext(options[i].__executionContext))
return true;}
return false;}
targetAdded(target){target.runtimeModel.executionContexts().forEach(this._executionContextCreated,this);}
targetRemoved(target){var executionContexts=this._optionByExecutionContext.keysArray();for(var i=0;i<executionContexts.length;++i){if(executionContexts[i].target()===target)
this._executionContextDestroyed(executionContexts[i]);}}
_select(option){this._selectElement.selectedIndex=Array.prototype.indexOf.call((this._selectElement),option);this._updateSelectionWarning();}
_selectedOption(){if(this._selectElement.selectedIndex>=0)
return this._selectElement[this._selectElement.selectedIndex];return null;}};;Console.ConsoleViewMessage=class{constructor(consoleMessage,linkifier,nestingLevel){this._message=consoleMessage;this._linkifier=linkifier;this._repeatCount=1;this._closeGroupDecorationCount=0;this._nestingLevel=nestingLevel;this._dataGrid=null;this._previewFormatter=new Components.RemoteObjectPreviewFormatter();this._searchRegex=null;}
_target(){return this.consoleMessage().target();}
element(){return this.toMessageElement();}
wasShown(){if(this._dataGrid)
this._dataGrid.updateWidths();this._isVisible=true;}
onResize(){if(!this._isVisible)
return;if(this._dataGrid)
this._dataGrid.onResize();}
willHide(){this._isVisible=false;this._cachedHeight=this.contentElement().offsetHeight;}
fastHeight(){if(this._cachedHeight)
return this._cachedHeight;const defaultConsoleRowHeight=19;if(this._message.type===SDK.ConsoleMessage.MessageType.Table){var table=this._message.parameters[0];if(table&&table.preview)
return defaultConsoleRowHeight*table.preview.properties.length;}
return defaultConsoleRowHeight;}
consoleMessage(){return this._message;}
_buildTableMessage(consoleMessage){var formattedMessage=createElement('span');UI.appendStyle(formattedMessage,'components/objectValue.css');formattedMessage.className='source-code';var anchorElement=this._buildMessageAnchor(consoleMessage);if(anchorElement)
formattedMessage.appendChild(anchorElement);var table=consoleMessage.parameters&&consoleMessage.parameters.length?consoleMessage.parameters[0]:null;if(table)
table=this._parameterToRemoteObject(table,this._target());if(!table||!table.preview)
return formattedMessage;var columnNames=[];var preview=table.preview;var rows=[];for(var i=0;i<preview.properties.length;++i){var rowProperty=preview.properties[i];var rowPreview=rowProperty.valuePreview;if(!rowPreview)
continue;var rowValue={};const maxColumnsToRender=20;for(var j=0;j<rowPreview.properties.length;++j){var cellProperty=rowPreview.properties[j];var columnRendered=columnNames.indexOf(cellProperty.name)!==-1;if(!columnRendered){if(columnNames.length===maxColumnsToRender)
continue;columnRendered=true;columnNames.push(cellProperty.name);}
if(columnRendered){var cellElement=this._renderPropertyPreviewOrAccessor(table,[rowProperty,cellProperty]);cellElement.classList.add('console-message-nowrap-below');rowValue[cellProperty.name]=cellElement;}}
rows.push([rowProperty.name,rowValue]);}
var flatValues=[];for(var i=0;i<rows.length;++i){var rowName=rows[i][0];var rowValue=rows[i][1];flatValues.push(rowName);for(var j=0;j<columnNames.length;++j)
flatValues.push(rowValue[columnNames[j]]);}
columnNames.unshift(Common.UIString('(index)'));if(flatValues.length){this._dataGrid=UI.SortableDataGrid.create(columnNames,flatValues);var formattedResult=createElementWithClass('span','console-message-text');var tableElement=formattedResult.createChild('div','console-message-formatted-table');var dataGridContainer=tableElement.createChild('span');tableElement.appendChild(this._formatParameter(table,true,false));dataGridContainer.appendChild(this._dataGrid.element);formattedMessage.appendChild(formattedResult);this._dataGrid.renderInline();}
return formattedMessage;}
_buildMessage(consoleMessage){var messageElement;var messageText=consoleMessage.messageText;if(consoleMessage.source===SDK.ConsoleMessage.MessageSource.ConsoleAPI){switch(consoleMessage.type){case SDK.ConsoleMessage.MessageType.Trace:messageElement=this._format(consoleMessage.parameters||['console.trace']);break;case SDK.ConsoleMessage.MessageType.Clear:messageElement=createElementWithClass('span','console-info');messageElement.textContent=Common.UIString('Console was cleared');break;case SDK.ConsoleMessage.MessageType.Assert:var args=[Common.UIString('Assertion failed:')];if(consoleMessage.parameters)
args=args.concat(consoleMessage.parameters);messageElement=this._format(args);break;case SDK.ConsoleMessage.MessageType.Dir:var obj=consoleMessage.parameters?consoleMessage.parameters[0]:undefined;var args=['%O',obj];messageElement=this._format(args);break;case SDK.ConsoleMessage.MessageType.Profile:case SDK.ConsoleMessage.MessageType.ProfileEnd:messageElement=this._format([messageText]);break;default:if(consoleMessage.parameters&&consoleMessage.parameters.length===1&&consoleMessage.parameters[0].type==='string')
messageElement=this._tryFormatAsError((consoleMessage.parameters[0].value));var args=consoleMessage.parameters||[messageText];messageElement=messageElement||this._format(args);}}else if(consoleMessage.source===SDK.ConsoleMessage.MessageSource.Network){if(consoleMessage.request){messageElement=createElement('span');if(consoleMessage.level===SDK.ConsoleMessage.MessageLevel.Error||consoleMessage.level===SDK.ConsoleMessage.MessageLevel.RevokedError){messageElement.createTextChild(consoleMessage.request.requestMethod+' ');messageElement.appendChild(Components.Linkifier.linkifyUsingRevealer(consoleMessage.request,consoleMessage.request.url,consoleMessage.request.url));if(consoleMessage.request.failed){messageElement.createTextChildren(' ',consoleMessage.request.localizedFailDescription);}else{messageElement.createTextChildren(' ',String(consoleMessage.request.statusCode),' (',consoleMessage.request.statusText,')');}}else{var fragment=Components.linkifyStringAsFragmentWithCustomLinkifier(messageText,linkifyRequest.bind(consoleMessage));messageElement.appendChild(fragment);}}else{messageElement=this._format([messageText]);}}else{if(consoleMessage.source===SDK.ConsoleMessage.MessageSource.Violation)
messageText=Common.UIString('[Violation] %s',messageText);var args=consoleMessage.parameters||[messageText];messageElement=this._format(args);}
messageElement.classList.add('console-message-text');var formattedMessage=createElement('span');UI.appendStyle(formattedMessage,'components/objectValue.css');formattedMessage.className='source-code';var anchorElement=this._buildMessageAnchor(consoleMessage);if(anchorElement)
formattedMessage.appendChild(anchorElement);formattedMessage.appendChild(messageElement);return formattedMessage;function linkifyRequest(title){return Components.Linkifier.linkifyUsingRevealer((this.request),title,this.request.url);}}
_buildMessageAnchor(consoleMessage){var anchorElement=null;if(consoleMessage.source!==SDK.ConsoleMessage.MessageSource.Network||consoleMessage.request){if(consoleMessage.scriptId){anchorElement=this._linkifyScriptId(consoleMessage.scriptId,consoleMessage.url||'',consoleMessage.line,consoleMessage.column);}else if(consoleMessage.stackTrace&&consoleMessage.stackTrace.callFrames.length){anchorElement=this._linkifyStackTraceTopFrame(consoleMessage.stackTrace);}else if(consoleMessage.url&&consoleMessage.url!=='undefined'){anchorElement=this._linkifyLocation(consoleMessage.url,consoleMessage.line,consoleMessage.column);}}else if(consoleMessage.url){anchorElement=Components.Linkifier.linkifyURLAsNode(consoleMessage.url,undefined,'console-message-url');}
if(anchorElement)
anchorElement.appendChild(createTextNode(' '));return anchorElement;}
_buildMessageWithStackTrace(consoleMessage,target,linkifier){var toggleElement=createElementWithClass('div','console-message-stack-trace-toggle');var contentElement=toggleElement.createChild('div','console-message-stack-trace-wrapper');var messageElement=this._buildMessage(consoleMessage);var clickableElement=contentElement.createChild('div');clickableElement.appendChild(messageElement);var stackTraceElement=contentElement.createChild('div');var stackTracePreview=Components.DOMPresentationUtils.buildStackTracePreviewContents(target,linkifier,consoleMessage.stackTrace);stackTraceElement.appendChild(stackTracePreview);stackTraceElement.classList.add('hidden');function expandStackTrace(expand){stackTraceElement.classList.toggle('hidden',!expand);toggleElement.classList.toggle('expanded',expand);}
function toggleStackTrace(event){var linkClicked=event.target&&event.target.enclosingNodeOrSelfWithNodeName('a');if(event.target.hasSelection()||linkClicked)
return;expandStackTrace(stackTraceElement.classList.contains('hidden'));event.consume();}
clickableElement.addEventListener('click',toggleStackTrace,false);if(consoleMessage.type===SDK.ConsoleMessage.MessageType.Trace)
expandStackTrace(true);toggleElement._expandStackTraceForTest=expandStackTrace.bind(null,true);return toggleElement;}
_linkifyLocation(url,lineNumber,columnNumber){var target=this._target();if(!target)
return null;return this._linkifier.linkifyScriptLocation(target,null,url,lineNumber,columnNumber,'console-message-url');}
_linkifyStackTraceTopFrame(stackTrace){var target=this._target();if(!target)
return null;return this._linkifier.linkifyStackTraceTopFrame(target,stackTrace,'console-message-url');}
_linkifyScriptId(scriptId,url,lineNumber,columnNumber){var target=this._target();if(!target)
return null;return this._linkifier.linkifyScriptLocation(target,scriptId,url,lineNumber,columnNumber,'console-message-url');}
_parameterToRemoteObject(parameter,target){if(parameter instanceof SDK.RemoteObject)
return parameter;if(!target)
return SDK.RemoteObject.fromLocalObject(parameter);if(typeof parameter==='object')
return target.runtimeModel.createRemoteObject(parameter);return target.runtimeModel.createRemoteObjectFromPrimitiveValue(parameter);}
_format(parameters){var formattedResult=createElement('span');if(!parameters.length)
return formattedResult;for(var i=0;i<parameters.length;++i)
parameters[i]=this._parameterToRemoteObject(parameters[i],this._target());var shouldFormatMessage=SDK.RemoteObject.type(((parameters))[0])==='string'&&(this._message.type!==SDK.ConsoleMessage.MessageType.Result||this._message.level===SDK.ConsoleMessage.MessageLevel.Error||this._message.level===SDK.ConsoleMessage.MessageLevel.RevokedError);if(shouldFormatMessage){var result=this._formatWithSubstitutionString((parameters[0].description),parameters.slice(1),formattedResult);parameters=result.unusedSubstitutions;if(parameters.length)
formattedResult.createTextChild(' ');}
for(var i=0;i<parameters.length;++i){if(shouldFormatMessage&&parameters[i].type==='string')
formattedResult.appendChild(Components.linkifyStringAsFragment(parameters[i].description));else
formattedResult.appendChild(this._formatParameter(parameters[i],false,true));if(i<parameters.length-1)
formattedResult.createTextChild(' ');}
return formattedResult;}
_formatParameter(output,forceObjectFormat,includePreview){if(output.customPreview())
return(new Components.CustomPreviewComponent(output)).element;var type=forceObjectFormat?'object':(output.subtype||output.type);var element;switch(type){case'array':case'typedarray':element=this._formatParameterAsArray(output);break;case'error':element=this._formatParameterAsError(output);break;case'function':case'generator':element=this._formatParameterAsFunction(output,includePreview);break;case'iterator':case'map':case'object':case'promise':case'proxy':case'set':element=this._formatParameterAsObject(output,includePreview);break;case'node':element=this._formatParameterAsNode(output);break;case'string':element=this._formatParameterAsString(output);break;case'boolean':case'date':case'null':case'number':case'regexp':case'symbol':case'undefined':element=this._formatParameterAsValue(output);break;default:element=this._formatParameterAsValue(output);console.error('Tried to format remote object of unknown type.');}
element.classList.add('object-value-'+type);element.classList.add('source-code');return element;}
_formatParameterAsValue(obj){var result=createElement('span');result.createTextChild(obj.description||'');if(obj.objectId)
result.addEventListener('contextmenu',this._contextMenuEventFired.bind(this,obj),false);return result;}
_formatParameterAsObject(obj,includePreview){var titleElement=createElement('span');if(includePreview&&obj.preview){titleElement.classList.add('console-object-preview');this._previewFormatter.appendObjectPreview(titleElement,obj.preview);}else if(obj.type==='function'){Components.ObjectPropertiesSection.formatObjectAsFunction(obj,titleElement,false);titleElement.classList.add('object-value-function');}else{titleElement.createTextChild(obj.description||'');}
var section=new Components.ObjectPropertiesSection(obj,titleElement,this._linkifier);section.element.classList.add('console-view-object-properties-section');section.enableContextMenu();return section.element;}
_formatParameterAsFunction(func,includePreview){var result=createElement('span');SDK.RemoteFunction.objectAsFunction(func).targetFunction().then(formatTargetFunction.bind(this));return result;function formatTargetFunction(targetFunction){var functionElement=createElement('span');Components.ObjectPropertiesSection.formatObjectAsFunction(targetFunction,functionElement,true,includePreview);result.appendChild(functionElement);if(targetFunction!==func){var note=result.createChild('span','object-info-state-note');note.title=Common.UIString('Function was resolved from bound function.');}
result.addEventListener('contextmenu',this._contextMenuEventFired.bind(this,targetFunction),false);}}
_contextMenuEventFired(obj,event){var contextMenu=new UI.ContextMenu(event);contextMenu.appendApplicableItems(obj);contextMenu.show();}
_renderPropertyPreviewOrAccessor(object,propertyPath){var property=propertyPath.peekLast();if(property.type==='accessor')
return this._formatAsAccessorProperty(object,propertyPath.map(property=>property.name),false);return this._previewFormatter.renderPropertyPreview(property.type,(property.subtype),property.value);}
_formatParameterAsNode(object){var result=createElement('span');Common.Renderer.renderPromise(object).then(appendRenderer.bind(this),failedToRender.bind(this));return result;function appendRenderer(rendererElement){result.appendChild(rendererElement);this._formattedParameterAsNodeForTest();}
function failedToRender(){result.appendChild(this._formatParameterAsObject(object,false));}}
_formattedParameterAsNodeForTest(){}
_formatParameterAsArray(array){var usePrintedArrayFormat=this._message.type!==SDK.ConsoleMessage.MessageType.DirXML&&this._message.type!==SDK.ConsoleMessage.MessageType.Result;var isLongArray=array.arrayLength()>100;if(usePrintedArrayFormat||isLongArray)
return this._formatParameterAsObject(array,usePrintedArrayFormat||!isLongArray);var result=createElement('span');array.getAllProperties(false,printArrayResult.bind(this));return result;function printArrayResult(properties){if(!properties){result.appendChild(this._formatParameterAsObject(array,false));return;}
var titleElement=createElement('span');var elements={};for(var i=0;i<properties.length;++i){var property=properties[i];var name=property.name;if(isNaN(name))
continue;if(property.getter)
elements[name]=this._formatAsAccessorProperty(array,[name],true);else if(property.value)
elements[name]=this._formatAsArrayEntry(property.value);}
titleElement.createTextChild('[');var lastNonEmptyIndex=-1;function appendUndefined(titleElement,index){if(index-lastNonEmptyIndex<=1)
return;var span=titleElement.createChild('span','object-value-undefined');span.textContent=Common.UIString('undefined × %d',index-lastNonEmptyIndex-1);}
var length=array.arrayLength();for(var i=0;i<length;++i){var element=elements[i];if(!element)
continue;if(i-lastNonEmptyIndex>1){appendUndefined(titleElement,i);titleElement.createTextChild(', ');}
titleElement.appendChild(element);lastNonEmptyIndex=i;if(i<length-1)
titleElement.createTextChild(', ');}
appendUndefined(titleElement,length);titleElement.createTextChild(']');var section=new Components.ObjectPropertiesSection(array,titleElement,this._linkifier);section.element.classList.add('console-view-object-properties-section');section.enableContextMenu();result.appendChild(section.element);}}
_formatParameterAsString(output){var span=createElement('span');span.appendChild(Components.linkifyStringAsFragment(output.description||''));var result=createElement('span');result.createChild('span','object-value-string-quote').textContent='"';result.appendChild(span);result.createChild('span','object-value-string-quote').textContent='"';return result;}
_formatParameterAsError(output){var result=createElement('span');var errorSpan=this._tryFormatAsError(output.description||'');result.appendChild(errorSpan?errorSpan:Components.linkifyStringAsFragment(output.description||''));return result;}
_formatAsArrayEntry(output){return this._previewFormatter.renderPropertyPreview(output.type,output.subtype,output.description);}
_formatAsAccessorProperty(object,propertyPath,isArrayEntry){var rootElement=Components.ObjectPropertyTreeElement.createRemoteObjectAccessorPropertySpan(object,propertyPath,onInvokeGetterClick.bind(this));function onInvokeGetterClick(result,wasThrown){if(!result)
return;rootElement.removeChildren();if(wasThrown){var element=rootElement.createChild('span');element.textContent=Common.UIString('<exception>');element.title=(result.description);}else if(isArrayEntry){rootElement.appendChild(this._formatAsArrayEntry(result));}else{const maxLength=100;var type=result.type;var subtype=result.subtype;var description='';if(type!=='function'&&result.description){if(type==='string'||subtype==='regexp')
description=result.description.trimMiddle(maxLength);else
description=result.description.trimEnd(maxLength);}
rootElement.appendChild(this._previewFormatter.renderPropertyPreview(type,subtype,description));}}
return rootElement;}
_formatWithSubstitutionString(format,parameters,formattedResult){var formatters={};function parameterFormatter(force,obj){return this._formatParameter(obj,force,false);}
function stringFormatter(obj){return obj.description;}
function floatFormatter(obj){if(typeof obj.value!=='number')
return'NaN';return obj.value;}
function integerFormatter(obj){if(typeof obj.value!=='number')
return'NaN';return Math.floor(obj.value);}
function bypassFormatter(obj){return(obj instanceof Node)?obj:'';}
var currentStyle=null;function styleFormatter(obj){currentStyle={};var buffer=createElement('span');buffer.setAttribute('style',obj.description);for(var i=0;i<buffer.style.length;i++){var property=buffer.style[i];if(isWhitelistedProperty(property))
currentStyle[property]=buffer.style[property];}}
function isWhitelistedProperty(property){var prefixes=['background','border','color','font','line','margin','padding','text','-webkit-background','-webkit-border','-webkit-font','-webkit-margin','-webkit-padding','-webkit-text'];for(var i=0;i<prefixes.length;i++){if(property.startsWith(prefixes[i]))
return true;}
return false;}
formatters.o=parameterFormatter.bind(this,false);formatters.s=stringFormatter;formatters.f=floatFormatter;formatters.i=integerFormatter;formatters.d=integerFormatter;formatters.c=styleFormatter;formatters.O=parameterFormatter.bind(this,true);formatters._=bypassFormatter;function append(a,b){if(b instanceof Node){a.appendChild(b);}else if(typeof b!=='undefined'){var toAppend=Components.linkifyStringAsFragment(String(b));if(currentStyle){var wrapper=createElement('span');wrapper.appendChild(toAppend);applyCurrentStyle(wrapper);for(var i=0;i<wrapper.children.length;++i)
applyCurrentStyle(wrapper.children[i]);toAppend=wrapper;}
a.appendChild(toAppend);}
return a;}
function applyCurrentStyle(element){for(var key in currentStyle)
element.style[key]=currentStyle[key];}
return String.format(format,parameters,formatters,formattedResult,append);}
matchesFilterRegex(regexObject){regexObject.lastIndex=0;var text=this.contentElement().deepTextContent();return regexObject.test(text);}
updateTimestamp(show){if(!this._contentElement)
return;if(show&&!this.timestampElement){this.timestampElement=createElementWithClass('span','console-timestamp');this.timestampElement.textContent=(new Date(this._message.timestamp)).toConsoleTime()+' ';this._contentElement.insertBefore(this.timestampElement,this._contentElement.firstChild);return;}
if(!show&&this.timestampElement){this.timestampElement.remove();delete this.timestampElement;}}
nestingLevel(){return this._nestingLevel;}
resetCloseGroupDecorationCount(){if(!this._closeGroupDecorationCount)
return;this._closeGroupDecorationCount=0;this._updateCloseGroupDecorations();}
incrementCloseGroupDecorationCount(){++this._closeGroupDecorationCount;this._updateCloseGroupDecorations();}
_updateCloseGroupDecorations(){if(!this._nestingLevelMarkers)
return;for(var i=0,n=this._nestingLevelMarkers.length;i<n;++i){var marker=this._nestingLevelMarkers[i];marker.classList.toggle('group-closed',n-i<=this._closeGroupDecorationCount);}}
contentElement(){if(this._contentElement)
return this._contentElement;var contentElement=createElementWithClass('div','console-message');this._contentElement=contentElement;if(this._message.type===SDK.ConsoleMessage.MessageType.StartGroup||this._message.type===SDK.ConsoleMessage.MessageType.StartGroupCollapsed)
contentElement.classList.add('console-group-title');var formattedMessage;var consoleMessage=this._message;var target=consoleMessage.target();var shouldIncludeTrace=!!consoleMessage.stackTrace&&(consoleMessage.source===SDK.ConsoleMessage.MessageSource.Network||consoleMessage.level===SDK.ConsoleMessage.MessageLevel.Error||consoleMessage.level===SDK.ConsoleMessage.MessageLevel.RevokedError||consoleMessage.type===SDK.ConsoleMessage.MessageType.Trace||consoleMessage.level===SDK.ConsoleMessage.MessageLevel.Warning);if(target&&shouldIncludeTrace)
formattedMessage=this._buildMessageWithStackTrace(consoleMessage,target,this._linkifier);else if(this._message.type===SDK.ConsoleMessage.MessageType.Table)
formattedMessage=this._buildTableMessage(this._message);else
formattedMessage=this._buildMessage(consoleMessage);contentElement.appendChild(formattedMessage);this.updateTimestamp(Common.moduleSetting('consoleTimestampsEnabled').get());return this._contentElement;}
toMessageElement(){if(this._element)
return this._element;this._element=createElement('div');this.updateMessageElement();return this._element;}
updateMessageElement(){if(!this._element)
return;this._element.className='console-message-wrapper';this._element.removeChildren();this._nestingLevelMarkers=[];for(var i=0;i<this._nestingLevel;++i)
this._nestingLevelMarkers.push(this._element.createChild('div','nesting-level-marker'));this._updateCloseGroupDecorations();this._element.message=this;switch(this._message.level){case SDK.ConsoleMessage.MessageLevel.Log:this._element.classList.add('console-log-level');break;case SDK.ConsoleMessage.MessageLevel.Debug:this._element.classList.add('console-debug-level');break;case SDK.ConsoleMessage.MessageLevel.Warning:this._element.classList.add('console-warning-level');break;case SDK.ConsoleMessage.MessageLevel.Error:this._element.classList.add('console-error-level');break;case SDK.ConsoleMessage.MessageLevel.RevokedError:this._element.classList.add('console-revokedError-level');break;case SDK.ConsoleMessage.MessageLevel.Info:this._element.classList.add('console-info-level');break;}
this._element.appendChild(this.contentElement());if(this._repeatCount>1)
this._showRepeatCountElement();}
repeatCount(){return this._repeatCount||1;}
resetIncrementRepeatCount(){this._repeatCount=1;if(!this._repeatCountElement)
return;this._repeatCountElement.remove();delete this._repeatCountElement;}
incrementRepeatCount(){this._repeatCount++;this._showRepeatCountElement();}
_showRepeatCountElement(){if(!this._contentElement)
return;if(!this._repeatCountElement){this._repeatCountElement=createElementWithClass('label','console-message-repeat-count','dt-small-bubble');switch(this._message.level){case SDK.ConsoleMessage.MessageLevel.Warning:this._repeatCountElement.type='warning';break;case SDK.ConsoleMessage.MessageLevel.Error:this._repeatCountElement.type='error';break;case SDK.ConsoleMessage.MessageLevel.Debug:this._repeatCountElement.type='debug';break;default:this._repeatCountElement.type='info';}
this._element.insertBefore(this._repeatCountElement,this._contentElement);this._contentElement.classList.add('repeated-message');}
this._repeatCountElement.textContent=this._repeatCount;}
get text(){return this._message.messageText;}
setSearchRegex(regex){if(this._searchHiglightNodeChanges&&this._searchHiglightNodeChanges.length)
UI.revertDomChanges(this._searchHiglightNodeChanges);this._searchRegex=regex;this._searchHighlightNodes=[];this._searchHiglightNodeChanges=[];if(!this._searchRegex)
return;var text=this.contentElement().deepTextContent();var match;this._searchRegex.lastIndex=0;var sourceRanges=[];while((match=this._searchRegex.exec(text))&&match[0])
sourceRanges.push(new Common.SourceRange(match.index,match[0].length));if(sourceRanges.length){this._searchHighlightNodes=UI.highlightSearchResults(this.contentElement(),sourceRanges,this._searchHiglightNodeChanges);}}
searchRegex(){return this._searchRegex;}
searchCount(){return this._searchHighlightNodes.length;}
searchHighlightNode(index){return this._searchHighlightNodes[index];}
_tryFormatAsError(string){function startsWith(prefix){return string.startsWith(prefix);}
var errorPrefixes=['EvalError','ReferenceError','SyntaxError','TypeError','RangeError','Error','URIError'];var target=this._target();if(!target||!errorPrefixes.some(startsWith))
return null;var debuggerModel=SDK.DebuggerModel.fromTarget(target);if(!debuggerModel)
return null;var lines=string.split('\n');var links=[];var position=0;for(var i=0;i<lines.length;++i){position+=i>0?lines[i-1].length+1:0;var isCallFrameLine=/^\s*at\s/.test(lines[i]);if(!isCallFrameLine&&links.length)
return null;if(!isCallFrameLine)
continue;var openBracketIndex=-1;var closeBracketIndex=-1;var match=/\([^\)\(]+\)/.exec(lines[i]);if(match){openBracketIndex=match.index;closeBracketIndex=match.index+match[0].length-1;}
var hasOpenBracket=openBracketIndex!==-1;var left=hasOpenBracket?openBracketIndex+1:lines[i].indexOf('at')+3;var right=hasOpenBracket?closeBracketIndex:lines[i].length;var linkCandidate=lines[i].substring(left,right);var splitResult=Common.ParsedURL.splitLineAndColumn(linkCandidate);if(!splitResult)
return null;var parsed=splitResult.url.asParsedURL();var url;if(parsed)
url=parsed.url;else if(debuggerModel.scriptsForSourceURL(splitResult.url).length)
url=splitResult.url;else if(splitResult.url==='<anonymous>')
continue;else
return null;links.push({url:url,positionLeft:position+left,positionRight:position+right,lineNumber:splitResult.lineNumber,columnNumber:splitResult.columnNumber});}
if(!links.length)
return null;var formattedResult=createElement('span');var start=0;for(var i=0;i<links.length;++i){formattedResult.appendChild(Components.linkifyStringAsFragment(string.substring(start,links[i].positionLeft)));formattedResult.appendChild(this._linkifier.linkifyScriptLocation(target,null,links[i].url,links[i].lineNumber,links[i].columnNumber));start=links[i].positionRight;}
if(start!==string.length)
formattedResult.appendChild(Components.linkifyStringAsFragment(string.substring(start)));return formattedResult;}};Console.ConsoleGroupViewMessage=class extends Console.ConsoleViewMessage{constructor(consoleMessage,linkifier,nestingLevel){console.assert(consoleMessage.isGroupStartMessage());super(consoleMessage,linkifier,nestingLevel);this.setCollapsed(consoleMessage.type===SDK.ConsoleMessage.MessageType.StartGroupCollapsed);}
setCollapsed(collapsed){this._collapsed=collapsed;if(this._element)
this._element.classList.toggle('collapsed',this._collapsed);}
collapsed(){return this._collapsed;}
toMessageElement(){if(!this._element){super.toMessageElement();this._element.classList.toggle('collapsed',this._collapsed);}
return this._element;}};;Console.ConsolePrompt=class extends UI.Widget{constructor(){super();this._addCompletionsFromHistory=true;this._history=new Console.ConsoleHistoryManager();this._initialText='';this._editor=null;this.element.tabIndex=0;self.runtime.extension(UI.TextEditorFactory).instance().then(gotFactory.bind(this));function gotFactory(factory){this._editor=factory.createEditor({lineNumbers:false,lineWrapping:true,mimeType:'javascript',autoHeight:true});this._editor.configureAutocomplete({substituteRangeCallback:this._substituteRange.bind(this),suggestionsCallback:this._wordsWithQuery.bind(this),captureEnter:true});this._editor.widget().element.addEventListener('keydown',this._editorKeyDown.bind(this),true);this._editor.widget().show(this.element);this.setText(this._initialText);delete this._initialText;if(this.hasFocus())
this.focus();this.element.tabIndex=-1;this._editorSetForTest();}}
history(){return this._history;}
clearAutocomplete(){if(this._editor)
this._editor.clearAutocomplete();}
_isCaretAtEndOfPrompt(){return!!this._editor&&this._editor.selection().collapseToEnd().equal(this._editor.fullRange().collapseToEnd());}
moveCaretToEndOfPrompt(){if(this._editor)
this._editor.setSelection(Common.TextRange.createFromLocation(Infinity,Infinity));}
setText(text){if(this._editor)
this._editor.setText(text);else
this._initialText=text;}
text(){return this._editor?this._editor.text():this._initialText;}
setAddCompletionsFromHistory(value){this._addCompletionsFromHistory=value;}
_editorKeyDown(event){var keyboardEvent=(event);var newText;var isPrevious;switch(keyboardEvent.keyCode){case UI.KeyboardShortcut.Keys.Up.code:if(this._editor.selection().endLine>0)
break;newText=this._history.previous(this.text());isPrevious=true;break;case UI.KeyboardShortcut.Keys.Down.code:if(this._editor.selection().endLine<this._editor.fullRange().endLine)
break;newText=this._history.next();break;case UI.KeyboardShortcut.Keys.P.code:if(Host.isMac()&&keyboardEvent.ctrlKey&&!keyboardEvent.metaKey&&!keyboardEvent.altKey&&!keyboardEvent.shiftKey){newText=this._history.previous(this.text());isPrevious=true;}
break;case UI.KeyboardShortcut.Keys.N.code:if(Host.isMac()&&keyboardEvent.ctrlKey&&!keyboardEvent.metaKey&&!keyboardEvent.altKey&&!keyboardEvent.shiftKey)
newText=this._history.next();break;case UI.KeyboardShortcut.Keys.Enter.code:this._enterKeyPressed(keyboardEvent);break;}
if(newText===undefined)
return;keyboardEvent.consume(true);this.setText(newText);if(isPrevious)
this._editor.setSelection(Common.TextRange.createFromLocation(0,Infinity));else
this.moveCaretToEndOfPrompt();}
_enterKeyPressed(event){if(event.altKey||event.ctrlKey||event.shiftKey)
return;event.consume(true);this.clearAutocomplete();var str=this.text();if(!str.length)
return;var currentExecutionContext=UI.context.flavor(SDK.ExecutionContext);if(!this._isCaretAtEndOfPrompt()||!currentExecutionContext){this._appendCommand(str,true);return;}
currentExecutionContext.target().runtimeModel.compileScript(str,'',false,currentExecutionContext.id,compileCallback.bind(this));function compileCallback(scriptId,exceptionDetails){if(str!==this.text())
return;if(exceptionDetails&&(exceptionDetails.exception.description.startsWith('SyntaxError: Unexpected end of input')||exceptionDetails.exception.description.startsWith('SyntaxError: Unterminated template literal'))){this._editor.newlineAndIndent();this._enterProcessedForTest();return;}
this._appendCommand(str,true);this._enterProcessedForTest();}}
_appendCommand(text,useCommandLineAPI){this.setText('');var currentExecutionContext=UI.context.flavor(SDK.ExecutionContext);if(currentExecutionContext){SDK.ConsoleModel.evaluateCommandInConsole(currentExecutionContext,text,useCommandLineAPI);if(Console.ConsolePanel.instance().isShowing())
Host.userMetrics.actionTaken(Host.UserMetrics.Action.CommandEvaluatedInConsolePanel);}}
_enterProcessedForTest(){}
_historyCompletions(prefix,force){if(!this._addCompletionsFromHistory||!this._isCaretAtEndOfPrompt()||(!prefix&&!force))
return[];var result=[];var text=this.text();var set=new Set();var data=this._history.historyData();for(var i=data.length-1;i>=0&&result.length<50;--i){var item=data[i];if(!item.startsWith(text))
continue;if(set.has(item))
continue;set.add(item);result.push({title:item.substring(text.length-prefix.length),className:'additional'});}
return result;}
focus(){if(this._editor)
this._editor.widget().focus();else
this.element.focus();}
_substituteRange(lineNumber,columnNumber){var lineText=this._editor.line(lineNumber);var index;for(index=lineText.length-1;index>=0;index--){if(' =:[({;,!+-*/&|^<>.'.indexOf(lineText.charAt(index))!==-1)
break;}
return new Common.TextRange(lineNumber,index+1,lineNumber,columnNumber);}
_wordsWithQuery(queryRange,substituteRange,force,currentTokenType){var query=this._editor.text(queryRange);var before=this._editor.text(new Common.TextRange(0,0,queryRange.startLine,queryRange.startColumn));var historyWords=this._historyCompletions(query,force);var excludedTokens=new Set(['js-comment','js-string-2']);if(!before.endsWith('['))
excludedTokens.add('js-string');if(excludedTokens.has(currentTokenType))
return Promise.resolve(historyWords);return Components.JavaScriptAutocomplete.completionsForTextInCurrentContext(before,query,force).then(words=>words.concat(historyWords));}
_editorSetForTest(){}};Console.ConsoleHistoryManager=class{constructor(){this._data=[];this._historyOffset=1;}
historyData(){return this._data;}
setHistoryData(data){this._data=data.slice();this._historyOffset=1;}
pushHistoryItem(text){if(this._uncommittedIsTop){this._data.pop();delete this._uncommittedIsTop;}
this._historyOffset=1;if(text===this._currentHistoryItem())
return;this._data.push(text);}
_pushCurrentText(currentText){if(this._uncommittedIsTop)
this._data.pop();this._uncommittedIsTop=true;this._data.push(currentText);}
previous(currentText){if(this._historyOffset>this._data.length)
return undefined;if(this._historyOffset===1)
this._pushCurrentText(currentText);++this._historyOffset;return this._currentHistoryItem();}
next(){if(this._historyOffset===1)
return undefined;--this._historyOffset;return this._currentHistoryItem();}
_currentHistoryItem(){return this._data[this._data.length-this._historyOffset];}};;Console.ConsoleView=class extends UI.VBox{constructor(){super();this.setMinimumSize(0,35);this.registerRequiredCSS('console/consoleView.css');this._searchableView=new UI.SearchableView(this);this._searchableView.setPlaceholder(Common.UIString('Find string in logs'));this._searchableView.setMinimalSearchQuerySize(0);this._searchableView.show(this.element);this._contentsElement=this._searchableView.element;this._contentsElement.classList.add('console-view');this._visibleViewMessages=[];this._urlToMessageCount={};this._hiddenByFilterCount=0;this._regexMatchRanges=[];this._executionContextComboBox=new UI.ToolbarComboBox(null,'console-context');this._executionContextComboBox.setMaxWidth(200);this._consoleContextSelector=new Console.ConsoleContextSelector(this._executionContextComboBox.selectElement());this._filter=new Console.ConsoleViewFilter(this);this._filter.addEventListener(Console.ConsoleViewFilter.Events.FilterChanged,this._updateMessageList.bind(this));this._filterBar=new UI.FilterBar('consoleView');this._preserveLogCheckbox=new UI.ToolbarCheckbox(Common.UIString('Preserve log'),Common.UIString('Do not clear log on page reload / navigation'),Common.moduleSetting('preserveConsoleLog'));this._progressToolbarItem=new UI.ToolbarItem(createElement('div'));var toolbar=new UI.Toolbar('',this._contentsElement);toolbar.appendToolbarItem(UI.Toolbar.createActionButton((UI.actionRegistry.action('console.clear'))));toolbar.appendToolbarItem(this._filterBar.filterButton());toolbar.appendToolbarItem(this._executionContextComboBox);toolbar.appendToolbarItem(this._preserveLogCheckbox);toolbar.appendToolbarItem(this._progressToolbarItem);this._filterBar.show(this._contentsElement);this._filter.addFilters(this._filterBar);this._viewport=new UI.ViewportControl(this);this._viewport.setStickToBottom(true);this._viewport.contentElement().classList.add('console-group','console-group-messages');this._contentsElement.appendChild(this._viewport.element);this._messagesElement=this._viewport.element;this._messagesElement.id='console-messages';this._messagesElement.classList.add('monospace');this._messagesElement.addEventListener('click',this._messagesClicked.bind(this),true);this._viewportThrottler=new Common.Throttler(50);this._filterStatusMessageElement=createElementWithClass('div','console-message');this._messagesElement.insertBefore(this._filterStatusMessageElement,this._messagesElement.firstChild);this._filterStatusTextElement=this._filterStatusMessageElement.createChild('span','console-info');this._filterStatusMessageElement.createTextChild(' ');var resetFiltersLink=this._filterStatusMessageElement.createChild('span','console-info link');resetFiltersLink.textContent=Common.UIString('Show all messages.');resetFiltersLink.addEventListener('click',this._filter.reset.bind(this._filter),true);this._topGroup=Console.ConsoleGroup.createTopGroup();this._currentGroup=this._topGroup;this._promptElement=this._messagesElement.createChild('div','source-code');this._promptElement.id='console-prompt';this._promptElement.addEventListener('input',this._promptInput.bind(this),false);var selectAllFixer=this._messagesElement.createChild('div','console-view-fix-select-all');selectAllFixer.textContent='.';this._showAllMessagesCheckbox=new UI.ToolbarCheckbox(Common.UIString('Show all messages'));this._showAllMessagesCheckbox.inputElement.checked=true;this._showAllMessagesCheckbox.inputElement.addEventListener('change',this._updateMessageList.bind(this),false);this._showAllMessagesCheckbox.element.classList.add('hidden');toolbar.appendToolbarItem(this._showAllMessagesCheckbox);this._registerShortcuts();this._messagesElement.addEventListener('contextmenu',this._handleContextMenuEvent.bind(this),false);Common.moduleSetting('monitoringXHREnabled').addChangeListener(this._monitoringXHREnabledSettingChanged,this);this._linkifier=new Components.Linkifier();this._consoleMessages=[];this._viewMessageSymbol=Symbol('viewMessage');this._consoleHistorySetting=Common.settings.createLocalSetting('consoleHistory',[]);this._prompt=new Console.ConsolePrompt();this._prompt.show(this._promptElement);this._prompt.element.addEventListener('keydown',this._promptKeyDown.bind(this),true);this._consoleHistoryAutocompleteSetting=Common.moduleSetting('consoleHistoryAutocomplete');this._consoleHistoryAutocompleteSetting.addChangeListener(this._consoleHistoryAutocompleteChanged,this);var historyData=this._consoleHistorySetting.get();this._prompt.history().setHistoryData(historyData);this._consoleHistoryAutocompleteChanged();this._updateFilterStatus();Common.moduleSetting('consoleTimestampsEnabled').addChangeListener(this._consoleTimestampsSettingChanged,this);this._registerWithMessageSink();SDK.targetManager.observeTargets(this);this._initConsoleMessages();UI.context.addFlavorChangeListener(SDK.ExecutionContext,this._executionContextChanged,this);this._messagesElement.addEventListener('mousedown',this._updateStickToBottomOnMouseDown.bind(this),false);this._messagesElement.addEventListener('mouseup',this._updateStickToBottomOnMouseUp.bind(this),false);this._messagesElement.addEventListener('mouseleave',this._updateStickToBottomOnMouseUp.bind(this),false);this._messagesElement.addEventListener('wheel',this._updateStickToBottomOnWheel.bind(this),false);}
static instance(){if(!Console.ConsoleView._instance)
Console.ConsoleView._instance=new Console.ConsoleView();return Console.ConsoleView._instance;}
static clearConsole(){for(var target of SDK.targetManager.targets()){target.runtimeModel.discardConsoleEntries();target.consoleModel.requestClearMessages();}}
searchableView(){return this._searchableView;}
_clearHistory(){this._consoleHistorySetting.set([]);this._prompt.history().setHistoryData([]);}
_consoleHistoryAutocompleteChanged(){this._prompt.setAddCompletionsFromHistory(this._consoleHistoryAutocompleteSetting.get());}
_initConsoleMessages(){var mainTarget=SDK.targetManager.mainTarget();var resourceTreeModel=mainTarget&&SDK.ResourceTreeModel.fromTarget(mainTarget);var resourcesLoaded=!resourceTreeModel||resourceTreeModel.cachedResourcesLoaded();if(!mainTarget||!resourcesLoaded){SDK.targetManager.addModelListener(SDK.ResourceTreeModel,SDK.ResourceTreeModel.Events.CachedResourcesLoaded,this._onResourceTreeModelLoaded,this);return;}
this._fetchMultitargetMessages();}
_onResourceTreeModelLoaded(event){var resourceTreeModel=event.target;if(resourceTreeModel.target()!==SDK.targetManager.mainTarget())
return;SDK.targetManager.removeModelListener(SDK.ResourceTreeModel,SDK.ResourceTreeModel.Events.CachedResourcesLoaded,this._onResourceTreeModelLoaded,this);this._fetchMultitargetMessages();}
_fetchMultitargetMessages(){SDK.multitargetConsoleModel.addEventListener(SDK.ConsoleModel.Events.ConsoleCleared,this._consoleCleared,this);SDK.multitargetConsoleModel.addEventListener(SDK.ConsoleModel.Events.MessageAdded,this._onConsoleMessageAdded,this);SDK.multitargetConsoleModel.addEventListener(SDK.ConsoleModel.Events.MessageUpdated,this._onConsoleMessageUpdated,this);SDK.multitargetConsoleModel.addEventListener(SDK.ConsoleModel.Events.CommandEvaluated,this._commandEvaluated,this);SDK.multitargetConsoleModel.messages().forEach(this._addConsoleMessage,this);this._viewport.invalidate();}
itemCount(){return this._visibleViewMessages.length;}
itemElement(index){return this._visibleViewMessages[index];}
fastHeight(index){return this._visibleViewMessages[index].fastHeight();}
minimumRowHeight(){return 16;}
targetAdded(target){this._viewport.invalidate();this._updateAllMessagesCheckbox();}
targetRemoved(target){this._updateAllMessagesCheckbox();}
_updateAllMessagesCheckbox(){var hasMultipleCotexts=SDK.targetManager.targets(SDK.Target.Capability.JS).length>1;this._showAllMessagesCheckbox.element.classList.toggle('hidden',!hasMultipleCotexts);}
_registerWithMessageSink(){Common.console.messages().forEach(this._addSinkMessage,this);Common.console.addEventListener(Common.Console.Events.MessageAdded,messageAdded,this);function messageAdded(event){this._addSinkMessage((event.data));}}
_addSinkMessage(message){var level=SDK.ConsoleMessage.MessageLevel.Debug;switch(message.level){case Common.Console.MessageLevel.Error:level=SDK.ConsoleMessage.MessageLevel.Error;break;case Common.Console.MessageLevel.Warning:level=SDK.ConsoleMessage.MessageLevel.Warning;break;}
var consoleMessage=new SDK.ConsoleMessage(null,SDK.ConsoleMessage.MessageSource.Other,level,message.text,undefined,undefined,undefined,undefined,undefined,undefined,undefined,message.timestamp);this._addConsoleMessage(consoleMessage);}
_consoleTimestampsSettingChanged(event){var enabled=(event.data);this._updateMessageList();this._consoleMessages.forEach(function(viewMessage){viewMessage.updateTimestamp(enabled);});}
_executionContextChanged(){this._prompt.clearAutocomplete();if(!this._showAllMessagesCheckbox.checked())
this._updateMessageList();}
willHide(){this._hidePromptSuggestBox();}
wasShown(){this._viewport.refresh();}
focus(){if(this._prompt.hasFocus())
return;this._prompt.moveCaretToEndOfPrompt();this._prompt.focus();}
restoreScrollPositions(){if(this._viewport.stickToBottom())
this._immediatelyScrollToBottom();else
super.restoreScrollPositions();}
onResize(){this._scheduleViewportRefresh();this._hidePromptSuggestBox();if(this._viewport.stickToBottom())
this._immediatelyScrollToBottom();for(var i=0;i<this._visibleViewMessages.length;++i)
this._visibleViewMessages[i].onResize();}
_hidePromptSuggestBox(){this._prompt.clearAutocomplete();}
_scheduleViewportRefresh(){function invalidateViewport(){if(this._muteViewportUpdates){this._maybeDirtyWhileMuted=true;return Promise.resolve();}
if(this._needsFullUpdate){this._updateMessageList();delete this._needsFullUpdate;}else{this._viewport.invalidate();}
return Promise.resolve();}
if(this._muteViewportUpdates){this._maybeDirtyWhileMuted=true;this._scheduleViewportRefreshForTest(true);return;}else{this._scheduleViewportRefreshForTest(false);}
this._viewportThrottler.schedule(invalidateViewport.bind(this));}
_scheduleViewportRefreshForTest(muted){}
_immediatelyScrollToBottom(){this._viewport.setStickToBottom(true);this._promptElement.scrollIntoView(true);}
_updateFilterStatus(){this._filterStatusTextElement.textContent=Common.UIString(this._hiddenByFilterCount===1?'%d message is hidden by filters.':'%d messages are hidden by filters.',this._hiddenByFilterCount);this._filterStatusMessageElement.style.display=this._hiddenByFilterCount?'':'none';}
_onConsoleMessageAdded(event){var message=(event.data);this._addConsoleMessage(message);}
_addConsoleMessage(message){function compareTimestamps(viewMessage1,viewMessage2){return SDK.ConsoleMessage.timestampComparator(viewMessage1.consoleMessage(),viewMessage2.consoleMessage());}
if(message.type===SDK.ConsoleMessage.MessageType.Command||message.type===SDK.ConsoleMessage.MessageType.Result){message.timestamp=this._consoleMessages.length?this._consoleMessages.peekLast().consoleMessage().timestamp:0;}
var viewMessage=this._createViewMessage(message);message[this._viewMessageSymbol]=viewMessage;var insertAt=this._consoleMessages.upperBound(viewMessage,compareTimestamps);var insertedInMiddle=insertAt<this._consoleMessages.length;this._consoleMessages.splice(insertAt,0,viewMessage);if(this._urlToMessageCount[message.url])
++this._urlToMessageCount[message.url];else
this._urlToMessageCount[message.url]=1;if(!insertedInMiddle){this._appendMessageToEnd(viewMessage);this._updateFilterStatus();this._searchableView.updateSearchMatchesCount(this._regexMatchRanges.length);}else{this._needsFullUpdate=true;}
this._scheduleViewportRefresh();this._consoleMessageAddedForTest(viewMessage);}
_onConsoleMessageUpdated(event){var message=(event.data);var viewMessage=message[this._viewMessageSymbol];if(viewMessage){viewMessage.updateMessageElement();this._updateMessageList();}}
_consoleMessageAddedForTest(viewMessage){}
_appendMessageToEnd(viewMessage){if(!this._filter.shouldBeVisible(viewMessage)){if(this._filter.shouldBeVisibleByDefault(viewMessage))
this._hiddenByFilterCount++;return;}
if(this._tryToCollapseMessages(viewMessage,this._visibleViewMessages.peekLast()))
return;var lastMessage=this._visibleViewMessages.peekLast();if(viewMessage.consoleMessage().type===SDK.ConsoleMessage.MessageType.EndGroup){if(lastMessage&&!this._currentGroup.messagesHidden())
lastMessage.incrementCloseGroupDecorationCount();this._currentGroup=this._currentGroup.parentGroup();return;}
if(!this._currentGroup.messagesHidden()){var originatingMessage=viewMessage.consoleMessage().originatingMessage();if(lastMessage&&originatingMessage&&lastMessage.consoleMessage()===originatingMessage)
lastMessage.toMessageElement().classList.add('console-adjacent-user-command-result');this._visibleViewMessages.push(viewMessage);this._searchMessage(this._visibleViewMessages.length-1);}
if(viewMessage.consoleMessage().isGroupStartMessage())
this._currentGroup=new Console.ConsoleGroup(this._currentGroup,viewMessage);this._messageAppendedForTests();}
_messageAppendedForTests(){}
_createViewMessage(message){var nestingLevel=this._currentGroup.nestingLevel();switch(message.type){case SDK.ConsoleMessage.MessageType.Command:return new Console.ConsoleCommand(message,this._linkifier,nestingLevel);case SDK.ConsoleMessage.MessageType.Result:return new Console.ConsoleCommandResult(message,this._linkifier,nestingLevel);case SDK.ConsoleMessage.MessageType.StartGroupCollapsed:case SDK.ConsoleMessage.MessageType.StartGroup:return new Console.ConsoleGroupViewMessage(message,this._linkifier,nestingLevel);default:return new Console.ConsoleViewMessage(message,this._linkifier,nestingLevel);}}
_consoleCleared(){this._currentMatchRangeIndex=-1;this._consoleMessages=[];this._updateMessageList();this._hidePromptSuggestBox();this._viewport.setStickToBottom(true);this._linkifier.reset();}
_handleContextMenuEvent(event){if(event.target.enclosingNodeOrSelfWithNodeName('a'))
return;var contextMenu=new UI.ContextMenu(event);if(event.target.isSelfOrDescendant(this._promptElement)){contextMenu.show();return;}
function monitoringXHRItemAction(){Common.moduleSetting('monitoringXHREnabled').set(!Common.moduleSetting('monitoringXHREnabled').get());}
contextMenu.appendCheckboxItem(Common.UIString('Log XMLHttpRequests'),monitoringXHRItemAction,Common.moduleSetting('monitoringXHREnabled').get());var sourceElement=event.target.enclosingNodeOrSelfWithClass('console-message-wrapper');var consoleMessage=sourceElement?sourceElement.message.consoleMessage():null;var filterSubMenu=contextMenu.appendSubMenuItem(Common.UIString('Filter'));if(consoleMessage&&consoleMessage.url){var menuTitle=Common.UIString.capitalize('Hide ^messages from %s',new Common.ParsedURL(consoleMessage.url).displayName);filterSubMenu.appendItem(menuTitle,this._filter.addMessageURLFilter.bind(this._filter,consoleMessage.url));}
filterSubMenu.appendSeparator();var unhideAll=filterSubMenu.appendItem(Common.UIString.capitalize('Unhide ^all'),this._filter.removeMessageURLFilter.bind(this._filter));filterSubMenu.appendSeparator();var hasFilters=false;for(var url in this._filter.messageURLFilters){filterSubMenu.appendCheckboxItem(String.sprintf('%s (%d)',new Common.ParsedURL(url).displayName,this._urlToMessageCount[url]),this._filter.removeMessageURLFilter.bind(this._filter,url),true);hasFilters=true;}
filterSubMenu.setEnabled(hasFilters||(consoleMessage&&consoleMessage.url));unhideAll.setEnabled(hasFilters);contextMenu.appendSeparator();contextMenu.appendAction('console.clear');contextMenu.appendAction('console.clear.history');contextMenu.appendItem(Common.UIString('Save as...'),this._saveConsole.bind(this));var request=consoleMessage?consoleMessage.request:null;if(request&&request.resourceType()===Common.resourceTypes.XHR){contextMenu.appendSeparator();contextMenu.appendItem(Common.UIString('Replay XHR'),request.replayXHR.bind(request));}
contextMenu.show();}
_saveConsole(){var url=SDK.targetManager.mainTarget().inspectedURL();var parsedURL=url.asParsedURL();var filename=String.sprintf('%s-%d.log',parsedURL?parsedURL.host:'console',Date.now());var stream=new Bindings.FileOutputStream();var progressIndicator=new UI.ProgressIndicator();progressIndicator.setTitle(Common.UIString('Writing file…'));progressIndicator.setTotalWork(this.itemCount());var chunkSize=350;var messageIndex=0;stream.open(filename,openCallback.bind(this));function openCallback(accepted){if(!accepted)
return;this._progressToolbarItem.element.appendChild(progressIndicator.element);writeNextChunk.call(this,stream);}
function writeNextChunk(stream,error){if(messageIndex>=this.itemCount()||error){stream.close();progressIndicator.done();return;}
var lines=[];for(var i=0;i<chunkSize&&i+messageIndex<this.itemCount();++i){var message=this.itemElement(messageIndex+i);var messageContent=message.contentElement().deepTextContent();for(var j=0;j<message.repeatCount();++j)
lines.push(messageContent);}
messageIndex+=i;stream.write(lines.join('\n')+'\n',writeNextChunk.bind(this));progressIndicator.setWorked(messageIndex);}}
_tryToCollapseMessages(lastMessage,viewMessage){if(!Common.moduleSetting('consoleTimestampsEnabled').get()&&viewMessage&&!lastMessage.consoleMessage().isGroupMessage()&&lastMessage.consoleMessage().isEqual(viewMessage.consoleMessage())){viewMessage.incrementRepeatCount();return true;}
return false;}
_updateMessageList(){this._topGroup=Console.ConsoleGroup.createTopGroup();this._currentGroup=this._topGroup;this._regexMatchRanges=[];this._hiddenByFilterCount=0;for(var i=0;i<this._visibleViewMessages.length;++i){this._visibleViewMessages[i].resetCloseGroupDecorationCount();this._visibleViewMessages[i].resetIncrementRepeatCount();}
this._visibleViewMessages=[];for(var i=0;i<this._consoleMessages.length;++i)
this._appendMessageToEnd(this._consoleMessages[i]);this._updateFilterStatus();this._searchableView.updateSearchMatchesCount(this._regexMatchRanges.length);this._viewport.invalidate();}
_monitoringXHREnabledSettingChanged(event){var enabled=(event.data);SDK.targetManager.targets().forEach(function(target){target.networkAgent().setMonitoringXHREnabled(enabled);});}
_messagesClicked(event){var targetElement=event.deepElementFromPoint();if(!targetElement||targetElement.isComponentSelectionCollapsed())
this.focus();var groupMessage=event.target.enclosingNodeOrSelfWithClass('console-group-title');if(!groupMessage)
return;var consoleGroupViewMessage=groupMessage.parentElement.message;consoleGroupViewMessage.setCollapsed(!consoleGroupViewMessage.collapsed());this._updateMessageList();}
_registerShortcuts(){this._shortcuts={};var shortcut=UI.KeyboardShortcut;var section=Components.shortcutsScreen.section(Common.UIString('Console'));var shortcutL=shortcut.makeDescriptor('l',UI.KeyboardShortcut.Modifiers.Ctrl);var keys=[shortcutL];if(Host.isMac()){var shortcutK=shortcut.makeDescriptor('k',UI.KeyboardShortcut.Modifiers.Meta);keys.unshift(shortcutK);}
section.addAlternateKeys(keys,Common.UIString('Clear console'));keys=[shortcut.makeDescriptor(shortcut.Keys.Tab),shortcut.makeDescriptor(shortcut.Keys.Right)];section.addRelatedKeys(keys,Common.UIString('Accept suggestion'));var shortcutU=shortcut.makeDescriptor('u',UI.KeyboardShortcut.Modifiers.Ctrl);this._shortcuts[shortcutU.key]=this._clearPromptBackwards.bind(this);section.addAlternateKeys([shortcutU],Common.UIString('Clear console prompt'));keys=[shortcut.makeDescriptor(shortcut.Keys.Down),shortcut.makeDescriptor(shortcut.Keys.Up)];section.addRelatedKeys(keys,Common.UIString('Next/previous line'));if(Host.isMac()){keys=[shortcut.makeDescriptor('N',shortcut.Modifiers.Alt),shortcut.makeDescriptor('P',shortcut.Modifiers.Alt)];section.addRelatedKeys(keys,Common.UIString('Next/previous command'));}
section.addKey(shortcut.makeDescriptor(shortcut.Keys.Enter),Common.UIString('Execute command'));}
_clearPromptBackwards(){this._prompt.setText('');}
_promptKeyDown(event){var keyboardEvent=(event);if(keyboardEvent.key==='PageUp'){this._updateStickToBottomOnWheel();return;}
var shortcut=UI.KeyboardShortcut.makeKeyFromEvent(keyboardEvent);var handler=this._shortcuts[shortcut];if(handler){handler();keyboardEvent.preventDefault();}}
_printResult(result,originatingConsoleMessage,exceptionDetails){if(!result)
return;var level=!!exceptionDetails?SDK.ConsoleMessage.MessageLevel.Error:SDK.ConsoleMessage.MessageLevel.Log;var message;if(!exceptionDetails){message=new SDK.ConsoleMessage(result.target(),SDK.ConsoleMessage.MessageSource.JS,level,'',SDK.ConsoleMessage.MessageType.Result,undefined,undefined,undefined,undefined,[result]);}else{message=SDK.ConsoleMessage.fromException(result.target(),exceptionDetails,SDK.ConsoleMessage.MessageType.Result,undefined,undefined);}
message.setOriginatingMessage(originatingConsoleMessage);result.target().consoleModel.addMessage(message);}
_commandEvaluated(event){var data=(event.data);this._prompt.history().pushHistoryItem(data.text);this._consoleHistorySetting.set(this._prompt.history().historyData().slice(-Console.ConsoleView.persistedHistorySize));this._printResult(data.result,data.commandMessage,data.exceptionDetails);}
elementsToRestoreScrollPositionsFor(){return[this._messagesElement];}
searchCanceled(){this._cleanupAfterSearch();for(var i=0;i<this._visibleViewMessages.length;++i){var message=this._visibleViewMessages[i];message.setSearchRegex(null);}
this._currentMatchRangeIndex=-1;this._regexMatchRanges=[];delete this._searchRegex;this._viewport.refresh();}
performSearch(searchConfig,shouldJump,jumpBackwards){this.searchCanceled();this._searchableView.updateSearchMatchesCount(0);this._searchRegex=searchConfig.toSearchRegex(true);this._regexMatchRanges=[];this._currentMatchRangeIndex=-1;if(shouldJump)
this._searchShouldJumpBackwards=!!jumpBackwards;this._searchProgressIndicator=new UI.ProgressIndicator();this._searchProgressIndicator.setTitle(Common.UIString('Searching…'));this._searchProgressIndicator.setTotalWork(this._visibleViewMessages.length);this._progressToolbarItem.element.appendChild(this._searchProgressIndicator.element);this._innerSearch(0);}
_cleanupAfterSearch(){delete this._searchShouldJumpBackwards;if(this._innerSearchTimeoutId){clearTimeout(this._innerSearchTimeoutId);delete this._innerSearchTimeoutId;}
if(this._searchProgressIndicator){this._searchProgressIndicator.done();delete this._searchProgressIndicator;}}
_searchFinishedForTests(){}
_innerSearch(index){delete this._innerSearchTimeoutId;if(this._searchProgressIndicator.isCanceled()){this._cleanupAfterSearch();return;}
var startTime=Date.now();for(;index<this._visibleViewMessages.length&&Date.now()-startTime<100;++index)
this._searchMessage(index);this._searchableView.updateSearchMatchesCount(this._regexMatchRanges.length);if(typeof this._searchShouldJumpBackwards!=='undefined'&&this._regexMatchRanges.length){this._jumpToMatch(this._searchShouldJumpBackwards?-1:0);delete this._searchShouldJumpBackwards;}
if(index===this._visibleViewMessages.length){this._cleanupAfterSearch();setTimeout(this._searchFinishedForTests.bind(this),0);return;}
this._innerSearchTimeoutId=setTimeout(this._innerSearch.bind(this,index),100);this._searchProgressIndicator.setWorked(index);}
_searchMessage(index){var message=this._visibleViewMessages[index];message.setSearchRegex(this._searchRegex);for(var i=0;i<message.searchCount();++i)
this._regexMatchRanges.push({messageIndex:index,matchIndex:i});}
jumpToNextSearchResult(){this._jumpToMatch(this._currentMatchRangeIndex+1);}
jumpToPreviousSearchResult(){this._jumpToMatch(this._currentMatchRangeIndex-1);}
supportsCaseSensitiveSearch(){return true;}
supportsRegexSearch(){return true;}
_jumpToMatch(index){if(!this._regexMatchRanges.length)
return;var matchRange;if(this._currentMatchRangeIndex>=0){matchRange=this._regexMatchRanges[this._currentMatchRangeIndex];var message=this._visibleViewMessages[matchRange.messageIndex];message.searchHighlightNode(matchRange.matchIndex).classList.remove(UI.highlightedCurrentSearchResultClassName);}
index=mod(index,this._regexMatchRanges.length);this._currentMatchRangeIndex=index;this._searchableView.updateCurrentMatchIndex(index);matchRange=this._regexMatchRanges[index];var message=this._visibleViewMessages[matchRange.messageIndex];var highlightNode=message.searchHighlightNode(matchRange.matchIndex);highlightNode.classList.add(UI.highlightedCurrentSearchResultClassName);this._viewport.scrollItemIntoView(matchRange.messageIndex);highlightNode.scrollIntoViewIfNeeded();}
_updateStickToBottomOnMouseDown(){this._muteViewportUpdates=true;this._viewport.setStickToBottom(false);if(this._waitForScrollTimeout){clearTimeout(this._waitForScrollTimeout);delete this._waitForScrollTimeout;}}
_updateStickToBottomOnMouseUp(){if(!this._muteViewportUpdates)
return;this._waitForScrollTimeout=setTimeout(updateViewportState.bind(this),200);function updateViewportState(){this._muteViewportUpdates=false;this._viewport.setStickToBottom(this._messagesElement.isScrolledToBottom());if(this._maybeDirtyWhileMuted){this._scheduleViewportRefresh();delete this._maybeDirtyWhileMuted;}
delete this._waitForScrollTimeout;this._updateViewportStickinessForTest();}}
_updateViewportStickinessForTest(){}
_updateStickToBottomOnWheel(){this._updateStickToBottomOnMouseDown();this._updateStickToBottomOnMouseUp();}
_promptInput(event){if(this.itemCount()!==0&&this._viewport.firstVisibleIndex()!==this.itemCount())
this._immediatelyScrollToBottom();}};Console.ConsoleView.persistedHistorySize=300;Console.ConsoleViewFilter=class extends Common.Object{constructor(view){super();this._messageURLFiltersSetting=Common.settings.createSetting('messageURLFilters',{});this._messageLevelFiltersSetting=Common.settings.createSetting('messageLevelFilters',{});this._view=view;this._messageURLFilters=this._messageURLFiltersSetting.get();this._filterChanged=this.dispatchEventToListeners.bind(this,Console.ConsoleViewFilter.Events.FilterChanged);}
addFilters(filterBar){this._textFilterUI=new UI.TextFilterUI(true);this._textFilterUI.addEventListener(UI.FilterUI.Events.FilterChanged,this._textFilterChanged,this);filterBar.addFilter(this._textFilterUI);this._hideNetworkMessagesCheckbox=new UI.CheckboxFilterUI('',Common.UIString('Hide network'),true,Common.moduleSetting('hideNetworkMessages'));this._hideViolationMessagesCheckbox=new UI.CheckboxFilterUI('',Common.UIString('Hide violations'),false,Common.moduleSetting('hideViolationMessages'));Common.moduleSetting('hideNetworkMessages').addChangeListener(this._filterChanged,this);Common.moduleSetting('hideViolationMessages').addChangeListener(this._filterChanged,this);filterBar.addFilter(this._hideNetworkMessagesCheckbox);filterBar.addFilter(this._hideViolationMessagesCheckbox);var levels=[{name:SDK.ConsoleMessage.MessageLevel.Error,label:Common.UIString('Errors')},{name:SDK.ConsoleMessage.MessageLevel.Warning,label:Common.UIString('Warnings')},{name:SDK.ConsoleMessage.MessageLevel.Info,label:Common.UIString('Info')},{name:SDK.ConsoleMessage.MessageLevel.Log,label:Common.UIString('Logs')},{name:SDK.ConsoleMessage.MessageLevel.Debug,label:Common.UIString('Debug')},{name:SDK.ConsoleMessage.MessageLevel.RevokedError,label:Common.UIString('Handled')}];this._levelFilterUI=new UI.NamedBitSetFilterUI(levels,this._messageLevelFiltersSetting);this._levelFilterUI.addEventListener(UI.FilterUI.Events.FilterChanged,this._filterChanged,this);filterBar.addFilter(this._levelFilterUI);}
_textFilterChanged(event){this._filterRegex=this._textFilterUI.regex();this._filterChanged();}
addMessageURLFilter(url){this._messageURLFilters[url]=true;this._messageURLFiltersSetting.set(this._messageURLFilters);this._filterChanged();}
removeMessageURLFilter(url){if(!url)
this._messageURLFilters={};else
delete this._messageURLFilters[url];this._messageURLFiltersSetting.set(this._messageURLFilters);this._filterChanged();}
get messageURLFilters(){return this._messageURLFilters;}
shouldBeVisible(viewMessage){var message=viewMessage.consoleMessage();var executionContext=UI.context.flavor(SDK.ExecutionContext);if(!message.target())
return true;if(!this._view._showAllMessagesCheckbox.checked()&&executionContext){if(message.target()!==executionContext.target())
return false;if(message.executionContextId&&message.executionContextId!==executionContext.id)
return false;}
if(Common.moduleSetting('hideNetworkMessages').get()&&viewMessage.consoleMessage().source===SDK.ConsoleMessage.MessageSource.Network)
return false;if(Common.moduleSetting('hideViolationMessages').get()&&viewMessage.consoleMessage().source===SDK.ConsoleMessage.MessageSource.Violation)
return false;if(viewMessage.consoleMessage().isGroupMessage())
return true;if(message.type===SDK.ConsoleMessage.MessageType.Result||message.type===SDK.ConsoleMessage.MessageType.Command)
return true;if(message.url&&this._messageURLFilters[message.url])
return false;if(message.level&&!this._levelFilterUI.accept(message.level))
return false;if(this._filterRegex){this._filterRegex.lastIndex=0;if(!viewMessage.matchesFilterRegex(this._filterRegex))
return false;}
return true;}
shouldBeVisibleByDefault(viewMessage){return viewMessage.consoleMessage().source!==SDK.ConsoleMessage.MessageSource.Violation;}
reset(){this._messageURLFilters={};this._messageURLFiltersSetting.set(this._messageURLFilters);this._messageLevelFiltersSetting.set({});this._view._showAllMessagesCheckbox.inputElement.checked=true;Common.moduleSetting('hideNetworkMessages').set(false);Common.moduleSetting('hideViolationMessages').set(true);this._textFilterUI.setValue('');this._filterChanged();}};Console.ConsoleViewFilter.Events={FilterChanged:Symbol('FilterChanged')};Console.ConsoleCommand=class extends Console.ConsoleViewMessage{constructor(message,linkifier,nestingLevel){super(message,linkifier,nestingLevel);}
contentElement(){if(!this._contentElement){this._contentElement=createElementWithClass('div','console-user-command');this._contentElement.message=this;this._formattedCommand=createElementWithClass('span','source-code');this._formattedCommand.textContent=this.text.replaceControlCharacters();this._contentElement.appendChild(this._formattedCommand);if(this._formattedCommand.textContent.length<Console.ConsoleCommand.MaxLengthToIgnoreHighlighter){var javascriptSyntaxHighlighter=new UI.DOMSyntaxHighlighter('text/javascript',true);javascriptSyntaxHighlighter.syntaxHighlightNode(this._formattedCommand).then(this._updateSearch.bind(this));}else{this._updateSearch();}}
return this._contentElement;}
_updateSearch(){this.setSearchRegex(this.searchRegex());}};Console.ConsoleCommand.MaxLengthToIgnoreHighlighter=10000;Console.ConsoleCommandResult=class extends Console.ConsoleViewMessage{constructor(message,linkifier,nestingLevel){super(message,linkifier,nestingLevel);}
contentElement(){var element=super.contentElement();element.classList.add('console-user-command-result');this.updateTimestamp(false);return element;}};Console.ConsoleGroup=class{constructor(parentGroup,groupMessage){this._parentGroup=parentGroup;this._nestingLevel=parentGroup?parentGroup.nestingLevel()+1:0;this._messagesHidden=groupMessage&&groupMessage.collapsed()||this._parentGroup&&this._parentGroup.messagesHidden();}
static createTopGroup(){return new Console.ConsoleGroup(null,null);}
messagesHidden(){return this._messagesHidden;}
nestingLevel(){return this._nestingLevel;}
parentGroup(){return this._parentGroup||this;}};Console.ConsoleView.ActionDelegate=class{handleAction(context,actionId){switch(actionId){case'console.show':Common.console.show();return true;case'console.clear':Console.ConsoleView.clearConsole();return true;case'console.clear.history':Console.ConsoleView.instance()._clearHistory();return true;}
return false;}};Console.ConsoleView.RegexMatchRange;;Console.ConsolePanel=class extends UI.Panel{constructor(){super('console');this._view=Console.ConsoleView.instance();}
static instance(){return(self.runtime.sharedInstance(Console.ConsolePanel));}
wasShown(){super.wasShown();var wrapper=Console.ConsolePanel.WrapperView._instance;if(wrapper&&wrapper.isShowing())
UI.inspectorView.setDrawerMinimized(true);this._view.show(this.element);}
willHide(){super.willHide();if(Console.ConsolePanel.WrapperView._instance)
Console.ConsolePanel.WrapperView._instance._showViewInWrapper();UI.inspectorView.setDrawerMinimized(false);}
searchableView(){return Console.ConsoleView.instance().searchableView();}};Console.ConsolePanel.WrapperView=class extends UI.VBox{constructor(){super();this.element.classList.add('console-view-wrapper');Console.ConsolePanel.WrapperView._instance=this;this._view=Console.ConsoleView.instance();}
wasShown(){if(!Console.ConsolePanel.instance().isShowing())
this._showViewInWrapper();else
UI.inspectorView.setDrawerMinimized(true);}
willHide(){UI.inspectorView.setDrawerMinimized(false);}
_showViewInWrapper(){this._view.show(this.element);}};Console.ConsolePanel.ConsoleRevealer=class{reveal(object){var consoleView=Console.ConsoleView.instance();if(consoleView.isShowing()){consoleView.focus();return Promise.resolve();}
UI.viewManager.showView('console-view');return Promise.resolve();}};;Runtime.cachedResources["console/consoleView.css"]="/*\n * Copyright (C) 2006, 2007, 2008 Apple Inc.  All rights reserved.\n * Copyright (C) 2009 Anthony Ricaud <rik@webkit.org>\n *\n * Redistribution and use in source and binary forms, with or without\n * modification, are permitted provided that the following conditions\n * are met:\n *\n * 1.  Redistributions of source code must retain the above copyright\n *     notice, this list of conditions and the following disclaimer.\n * 2.  Redistributions in binary form must reproduce the above copyright\n *     notice, this list of conditions and the following disclaimer in the\n *     documentation and/or other materials provided with the distribution.\n * 3.  Neither the name of Apple Computer, Inc. (\"Apple\") nor the names of\n *     its contributors may be used to endorse or promote products derived\n *     from this software without specific prior written permission.\n *\n * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS \"AS IS\" AND ANY\n * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY\n * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n */\n\n.console-view {\n    background-color: white;\n    overflow: hidden;\n}\n\n.console-view > .toolbar {\n    border-bottom: 1px solid #dadada;\n}\n\n.console-view-wrapper {\n    background-color: #eee;\n}\n\n.console-view-fix-select-all {\n    height: 0;\n    overflow: hidden;\n}\n\n#console-messages {\n    flex: 1 1;\n    padding: 2px 0;\n    overflow-y: auto;\n    word-wrap: break-word;\n    -webkit-user-select: text;\n    transform: translateZ(0);\n}\n\n#console-prompt {\n    clear: right;\n    position: relative;\n    margin: 0 22px 0 20px;\n    min-height: 18px;  /* Sync with ConsoleViewMessage.js */\n}\n\n#console-prompt .CodeMirror {\n    padding: 3px 0 1px 0;\n}\n\n#console-prompt .CodeMirror-line {\n    padding-top: 0;\n}\n\n#console-prompt .CodeMirror-lines {\n    padding-top: 0;\n}\n\n#console-prompt::before {\n    background-position: -20px -20px;\n    margin-left: 4px;\n}\n\n.console-log-level .console-user-command-result::before {\n    background-position: -40px -20px;\n}\n\n.console-message,\n.console-user-command {\n    clear: right;\n    position: relative;\n    padding: 3px 22px 1px 0;\n    margin-left: 24px;\n    min-height: 18px;  /* Sync with ConsoleViewMessage.js */\n    flex: auto;\n    display: flex;\n}\n\n.console-message > * {\n    flex: auto;\n}\n\n.console-adjacent-user-command-result + .console-user-command-result.console-log-level::before {\n    background-image: none;\n}\n\n.console-timestamp {\n    color: gray;\n    -webkit-user-select: none;\n    flex-grow: 0;\n    margin-right: 5px;\n}\n\n.console-message::before,\n.console-user-command::before,\n#console-prompt::before,\n.console-group-title::before {\n    position: absolute;\n    display: block;\n    content: \"\";\n    left: -17px;\n    top: 9px;\n    width: 10px;\n    height: 10px;\n    margin-top: -4px;\n    -webkit-user-select: none;\n    background-image: url(Images/smallIcons.png);\n    background-size: 190px 30px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.1) {\n.console-message::before,\n.console-user-command::before,\n#console-prompt::before,\n.console-group-title::before {\n    background-image: url(Images/smallIcons_2x.png);\n}\n} /* media */\n\n.console-message-repeat-count {\n    margin: 2px 0 0 10px;\n    flex: none;\n}\n\n.repeated-message {\n    margin-left: 4px;\n}\n\n.console-error-level .repeated-message::before,\n.console-revokedError-level .repeated-message::before,\n.console-warning-level .repeated-message::before,\n.console-debug-level .repeated-message::before,\n.console-info-level .repeated-message::before {\n    visibility: hidden;\n}\n\n.repeated-message .console-message-stack-trace-toggle,\n.repeated-message > .console-message-text {\n    flex: 1;\n}\n\n.console-error-level .repeated-message,\n.console-revokedError-level .repeated-message,\n.console-log-level .repeated-message,\n.console-warning-level .repeated-message,\n.console-debug-level .repeated-message,\n.console-info-level .repeated-message {\n    display: flex;\n}\n\n.console-info {\n    color: rgb(128, 128, 128);\n    font-style: italic;\n}\n\n.console-group .console-group > .console-group-messages {\n    margin-left: 16px;\n}\n\n.console-group-title {\n    font-weight: bold;\n}\n\n.console-group-title::before {\n    -webkit-user-select: none;\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs.png);\n    -webkit-mask-size: 352px 168px;\n    float: left;\n    width: 8px;\n    content: \"a\";\n    color: transparent;\n    text-shadow: none;\n    margin-left: 3px;\n    margin-top: -7px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.1) {\n.console-group-title::before {\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.console-group .console-group-title::before {\n    -webkit-mask-position: -20px -96px;\n    background-color: rgb(110, 110, 110);\n}\n\n.console-message-wrapper.collapsed .console-group-title::before {\n    -webkit-mask-position: -4px -96px;\n}\n\n.console-group {\n    position: relative;\n}\n\n.console-message-wrapper {\n    display: flex;\n    border-bottom: 1px solid rgb(240, 240, 240);\n}\n\n.console-message-wrapper.console-adjacent-user-command-result {\n    border-bottom: none;\n}\n\n.console-message-wrapper.console-error-level {\n    border-top: 1px solid hsl(0, 100%, 92%);\n    border-bottom: 1px solid hsl(0, 100%, 92%);\n    margin-top: -1px;\n}\n\n.console-message-wrapper.console-warning-level {\n    border-top: 1px solid hsl(50, 100%, 88%);\n    border-bottom: 1px solid hsl(50, 100%, 88%);\n    margin-top: -1px;\n}\n\n.console-message-wrapper .nesting-level-marker {\n    width: 14px;\n    flex: 0 0 auto;\n    border-right: 1px solid #a5a5a5;\n    position: relative;\n    margin-bottom: -1px;\n}\n\n.console-message-wrapper:last-child .nesting-level-marker::before,\n.console-message-wrapper .nesting-level-marker.group-closed::before {\n    content: \"\";\n}\n\n.console-message-wrapper .nesting-level-marker::before {\n    border-bottom: 1px solid #a5a5a5;\n    position: absolute;\n    top: 0;\n    left: 0;\n    margin-left: 100%;\n    width: 3px;\n    height: 100%;\n    box-sizing: border-box;\n}\n\n.console-error-level {\n    background-color: hsl(0, 100%, 97%);\n}\n\n.-theme-with-dark-background .console-error-level {\n    background-color: hsl(0, 100%, 8%);\n}\n\n.console-warning-level {\n    background-color: hsl(50, 100%, 95%);\n}\n\n.-theme-with-dark-background .console-warning-level {\n    background-color: hsl(50, 100%, 10%);\n}\n\n.console-warning-level .console-message-text {\n    color: hsl(39, 100%, 18%);\n}\n\n.console-error-level .console-message-text,\n.console-error-level .console-view-object-properties-section {\n    color: red !important;\n}\n\n.-theme-with-dark-background .console-error-level .console-message-text,\n.-theme-with-dark-background .console-error-level .console-view-object-properties-section {\n    color: hsl(0, 100%, 75%) !important;\n}\n\n.console-debug-level .console-message-text {\n    color: blue;\n}\n\n.-theme-with-dark-background .console-debug-level .console-message-text {\n    color: hsl(220, 100%, 65%) !important;\n}\n\n.console-message.console-warning-level {\n    background-color: rgb(255, 250, 224);\n}\n\n.console-error-level .console-message::before,\n.console-revokedError-level .console-message::before,\n.console-warning-level .console-message::before,\n.console-debug-level .console-message::before,\n.console-info-level .console-message::before {\n    background-image: url(Images/smallIcons.png);\n    background-size: 190px 30px;\n    width: 10px;\n    height: 10px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.1) {\n.console-error-level .console-message::before,\n.console-revokedError-level .console-message::before,\n.console-warning-level .console-message::before,\n.console-debug-level .console-message::before,\n.console-info-level .console-message::before {\n    background-image: url(Images/smallIcons_2x.png);\n}\n} /* media */\n\n.console-warning-level .console-message::before {\n    background-position: -60px 0;\n}\n\n.console-error-level .console-message::before {\n    background-position: -20px 0;\n}\n\n.console-revokedError-level .console-message::before {\n    background-position: -40px 0;\n}\n\n.console-info-level .console-message::before {\n    background-position: -80px 0;\n}\n\n.console-user-command .console-message {\n    margin-left: -24px;\n    padding-right: 0;\n    border-bottom: none;\n}\n\n.console-user-command::before {\n    background-position: 0 -20px;\n}\n\n#console-messages .link {\n    text-decoration: underline;\n}\n\n#console-messages .link,\n#console-messages a {\n    color: rgb(33%, 33%, 33%);\n    cursor: pointer;\n    word-break: break-all;\n}\n\n#console-messages .link:hover,\n#console-messages a:hover {\n    color: rgb(15%, 15%, 15%);\n}\n\n.console-group-messages .section {\n    margin: 0 0 0 12px !important;\n}\n\n.console-group-messages .section > .header {\n    padding: 0 8px 0 0;\n    background-image: none;\n    border: none;\n    min-height: 0;\n}\n\n.console-group-messages .section > .header::before {\n    margin-left: -12px;\n}\n\n.console-group-messages .section > .header .title {\n    color: #222;\n    font-weight: normal;\n    line-height: 13px;\n}\n\n.console-group-messages .section .properties li .info {\n    padding-top: 0;\n    padding-bottom: 0;\n    color: rgb(60%, 60%, 60%);\n}\n\n.console-object-preview {\n    font-style: italic;\n    white-space: normal;\n    word-wrap: break-word;\n}\n\n.console-object-preview .name {\n    /* Follows .section .properties .name, .event-properties .name */\n    color: rgb(136, 19, 145);\n    flex-shrink: 0;\n}\n\n.console-message-text .object-value-string {\n    white-space: pre-wrap;\n}\n\n.console-message-formatted-table {\n    clear: both;\n}\n\n.console-message-url {\n    float: right;\n    text-align: right;\n    max-width: 100%;\n    margin-left: 4px;\n}\n\n.console-message-nowrap-below,\n.console-message-nowrap-below div,\n.console-message-nowrap-below span {\n    white-space: nowrap !important;\n}\n\n.object-state-note {\n    display: inline-block;\n    width: 11px;\n    height: 11px;\n    color: white;\n    text-align: center;\n    border-radius: 3px;\n    line-height: 13px;\n    margin: 0 6px;\n    font-size: 9px;\n}\n\n.-theme-with-dark-background .object-state-note {\n    background-color: hsl(230, 100%, 80%);\n}\n\n.info-note {\n    background-color: rgb(179, 203, 247);\n}\n\n.info-note::before {\n    content: \"i\";\n}\n\n.console-view-object-properties-section {\n    padding: 0px;\n}\n\n.console-message-stack-trace-toggle {\n    display: flex;\n    flex-direction: row;\n    align-items: flex-start;\n}\n\n.console-message-stack-trace-wrapper {\n    flex: 1 1 auto;\n    display: flex;\n    flex-direction: column;\n    align-items: stretch;\n}\n\n.console-message-stack-trace-wrapper > * {\n    flex: none;\n}\n\n.console-message-stack-trace-toggle .console-message-text::before {\n    content: \" \";\n    -webkit-user-select: none;\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs.png);\n    -webkit-mask-size: 352px 168px;\n    color: transparent;\n    text-shadow: none;\n    padding-right: 8px;\n    height: 12px;\n    background-color: rgb(110, 110, 110);\n    width: 12px;\n    flex: none;\n    -webkit-mask-position: -4px -96px;\n}\n\n@media (-webkit-min-device-pixel-ratio: 1.1) {\n.console-message-stack-trace-toggle .console-message-text::before {\n    -webkit-mask-image: url(Images/toolbarButtonGlyphs_2x.png);\n}\n} /* media */\n\n.console-message-stack-trace-toggle.expanded .console-message-text::before {\n    -webkit-mask-position: -20px -96px;\n}\n\n/*# sourceURL=console/consoleView.css */";