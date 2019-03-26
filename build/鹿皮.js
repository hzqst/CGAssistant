var cga = require('./cgaapi')(function(){
	console.log('开始执行模块：采集鹿皮，地点：伊尔村外 596,247');
	var stats = new cga.gather.stats('鹿皮', 40);
	var skill = cga.findPlayerSkill('狩猎');
	var skill2 = cga.findPlayerSkill('狩猎体验');
	if(!skill && !skill2){
		throw new Error('你没有狩猎或狩猎体验技能');
		return;
	}
	var useskill = skill2 ? skill2 : skill;
	var doWork = null;
	
	var exchange = (cb2)=>{
		stats.printStats();
		cga.travel.falan.toBank(()=>{
			cga.AsyncWaitNPCDialog(function(dlg){
				cga.saveToBankAll('银条', (r)=>{
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
						again();
					}, 3000);
				});
				return;
			}
			var items = cga.getInventoryItems();
			if(items.length >= 20)
			{
				clearInterval(tt);
				console.log('物品栏已满');
				exchange(cb2);
				return;
			}
			for(var i in items){
				if(items[i].name == '蕃茄' && items[i].count == 40)
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
		cga.travel.falan.toYiErCun(function(r){
			if(!r){
				cb(r);
				return;
			}
			const walkTo = [
				[19, 16, null],
				[16, 19, null],
				[12, 19, null],
				[12, 17, '村长的家'],
				[15, 16, null],
				[13, 13, null],
				[6, 13, '伊尔村'],
				[45, 79, null],
				[49, 75, null],
				[49, 53, null],
				[52, 48, null],
				[46, 40, null],
				[44, 35, null],
				[44, 31, null],
				[45, 31, '芙蕾雅'],
				[685, 340, null],
				[682, 325, null],
				[649, 288, null],
			];
			
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(r);
					return;
				}
				var tt = cga.keepStartWork(useskill.index, 0);
				waitEnd((r)=>{
					if(r){
						doWork();
						return;
					}
				}, tt);
			});
		});
	}
	
	doWork();
});