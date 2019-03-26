var cga = require('./cgaapi')(function(){
	console.log('开始执行模块：采集黄月木，地点：法兰外 361,180');
	var stats = new cga.gather.stats('黄月木', 40);
	var skill = cga.findPlayerSkill('伐木');
	if(!skill){
		throw new Error('你没有伐木技能');
		return;
	}
	if(skill.lv < 3){
		throw new Error('伐木等级不够，采集黄月木需要3级伐木');
		return;
	}
	var doWork = null;
	var exchange = (cb2)=>{
		stats.printStats();
		cga.travel.falan.toBank(()=>{
			cga.AsyncWaitNPCDialog(function(dlg){
				cga.saveToBankAll('黄月木', (r)=>{
					cb2(r);
				});
			});
		});
	}
	var waitEnd = function(cb2, tt){
		cga.AsyncWaitWorkingResult(function(r){
			var playerInfo = cga.GetPlayerInfo();
			if(playerInfo.mp == 0)
			{
				clearInterval(tt);
				console.log('蓝量耗尽');
				cga.travel.falan.toCastleHospital(()=>{
					setTimeout(()=>{
						doWork();
					}, 3000);
				});
				return;
			}
			var items = cga.getInventoryItems();
			if(items.length >= 20)
			{
				clearInterval(tt);
				console.log('物品栏已满，去银行存');
				exchange(cb2);
				return;
			}
			for(var i in items){
				if(items[i].name == '印度轻木' && items[i].count == 40)
					cga.DropItem(items[i].pos);
			}
			waitEnd(cb2);
		}, 10000);
	}
	doWork = ()=>{
		var playerInfo = cga.GetPlayerInfo();
		if(playerInfo.mp < playerInfo.maxmp)
		{
			console.log('去补蓝');
			cga.travel.falan.toCastleHospital(()=>{
				setTimeout(()=>{
					doWork();
				}, 3000);
			});
			return;
		}
		cga.travel.falan.toStone('W1', function(r){
			if(!r){
				cb(r);
				return;
			}
			const walkTo = [
				[59, 85, null],
				[54, 87, null],
				[23, 87, null],
				[22, 87, '芙蕾雅'],
				[368, 178, null],
				[361, 178, null],
				[361, 180, null]
			];
			
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(r);
					return;
				}
				var tt = cga.keepStartWork(skill.index, 0);
				waitEnd(()=>{
					if(r){
						doWork();
						return;
					}
				}, tt);
			});
		});
	}
	
	
});