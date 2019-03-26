var cga = require('./cgaapi')(function(){
	var taskObj = new cga.task.joinJobBattleCommon('剑士', cb);
	//剑士推荐信
	taskObj.requirements.push(function(){ return cga.getItemCount('#18101') > 0 ? true : false; });

	taskObj.doTask(()=>{
		console.log('ok');
	});
});