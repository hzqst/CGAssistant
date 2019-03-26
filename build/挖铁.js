var cga = require('./cgaapi')(function(){
	console.log('开始执行模块：采集铁矿，地点：国营24坑道地下四楼东北区');
	var stats = new cga.gather.stats('铁条', 40);
	var skill = cga.findPlayerSkill('挖掘');
	if(!skill){
		throw new Error('你没有挖掘技能');
		return;
	}
	if(skill.lv < 3){
		throw new Error('挖掘等级不够，采集铁矿需要2级挖掘');
		return;
	}
	var doWork = null;
	var exchange = (cb2)=>{
			cga.travel.falan.toMineStore('铁', ()=>{
			cga.AsyncWaitNPCDialog(function(dlg){
				cga.ClickNPCDialog(0, 0);
				cga.AsyncWaitNPCDialog(function(dlg2){
					var exchangeCount = cga.getItemCount('铁') / 20;
					var r = cga.BuyNPCStore([{index:0, count:exchangeCount}]);
					cga.AsyncWaitNPCDialog(function(dlg3){
						cga.travel.falan.toBank(()=>{
							cga.AsyncWaitNPCDialog(function(dlg){
								cga.saveToBankAll('铁条', (r)=>{
									cb2(r);
								});
							});
						});
					});
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
				console.log('物品栏已满，去换矿');
				exchange(cb2);
				return;
			}
			waitEnd(cb2);
		}, 10000);
	}
	
	doWork = ()=>{
		cga.travel.falan.toStone('W1', function(r){
			if(!r){
				console.log('前往法兰传送石W1失败');
				return;
			}
			const walkTo = [
				[59, 85, null],
				[54, 87, null],
				[23, 87, null],
				[22, 87, '芙蕾雅'],
				[370, 189, null],
				[363, 159, null],
				[352, 146, null],
				[351, 145, '国营24坑道 地下1楼'],
				[9, 17, null],
				[15, 13, null],
				[20, 8, null],
				[31, 10, null],
				[29, 14, null],
				[24, 17, null],
				[22, 22, '国营24坑道 地下2楼'],
				[22, 21, null],
			];
			const walkTo2 = [
				[23, 14, null],
				[23, 13, '国营24坑道 地下3楼'],
				[35, 32, null],
				[24, 31, null],
				[19, 28, null],
				[18, 23, null],
				[22, 19, null],
				[21, 10, null],
				[28, 3, null],
				[29, 3, '国营24坑道 地下4楼'],
				[33, 40, null],
				[27, 46, null],
				[19, 36, null],
			];
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(r);
					return;
				}
				cga.TurnTo(22, 20);
				setTimeout(()=>{
					cga.walkList(walkTo2, function(r){
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
				}, 1000);				
			});
		});
	}
	
	doWork();
});