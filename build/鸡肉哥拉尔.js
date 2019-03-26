var cga = require('./cgaapi')(function(){
	console.log('开始执行模块：采集鸡肉，地点：哥拉尔外 281,377');
	var stats = new cga.gather.stats('鸡肉', 40);
	var skill = cga.findPlayerSkill('狩猎');
	if(!skill){
		throw new Error('你没有狩猎技能');
		return;
	}
	if(skill.lv < 1){
		throw new Error('狩猎等级不够，采集鸡肉需要4级狩猎');
		return;
	}
	var again = null;
	var waitEnd = function(cb2){
		cga.AsyncWaitWorkingResult(function(r){
			var playerInfo = cga.GetPlayerInfo();
			if(playerInfo.mp == 0)
			{
				console.log('蓝量耗尽');
				cga.travel.gelaer.toHospital(()=>{
					setTimeout(()=>{
						again();
					}, 3000);
				});
				return;
			}
			var items = cga.getInventoryItems();
			if(items.length >= 20)
			{
				console.log('物品栏已满，采集结束');
				cb2();
				return;
			}
			waitEnd(cb2);
		}, 10000);
	}
	again = ()=>{
		cga.travel.gelaer.toStone('N', function(r){
			if(!r){
				throw new Error('前往哥拉尔N传送石失败');
				return;
			}
			const walkTo = [
				[117, 106, null],
				[119, 102, null],
				[119, 39, null],
				[119, 38, '库鲁克斯岛'],
				[281, 377, null],
			];
			
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(r);
					return;
				}
				var tt = cga.keepStartWork(skill.index, 0);
				waitEnd(()=>{
					stats.printStats();
					clearInterval(tt);
				});
			});
		});
	}
	
	again();
});