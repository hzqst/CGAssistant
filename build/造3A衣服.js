var cga = require('./cgaapi')(function(){
	
	var docraft = (cb)=>{
		if(!cga.findPlayerSkill('制衣服')){
			cb(false, '你没有制衣服技能');
			return;
		}
		if(cga.getItemCount('麻布') < 5){
			cga.craft.buyFabricLv1(0, 5, (r)=>{
				docraft(cb);
			});
			return;
		}
		if(cga.getItemCount('木棉布') < 5){
			cga.craft.buyFabricLv1(1, 5, (r)=>{
				docraft(cb);
			});
			return;
		}
		if(cga.getItemCount('毛毡') < 6){
			cga.craft.buyFabricLv1(2, 6, (r)=>{
				docraft(cb);
			});
			return;
		}
		var r = cga.craftNamedItem('硬皮服');
		if(r != true){
			cb(false, r);
			return;
		}
		cga.AsyncWaitWorkingResult(function(r){
			setTimeout(function(){//delay a bit
				cb(r);
			}, 100);
		}, 30000);
	}

	
	var doWork = (r, err)=>{
		if(!r){
			console.log(err);
			return;
		}
		if(cga.GetPlayerInfo().mp < 53){
			console.log('蓝不够，去补蓝');
			cga.travel.falan.toCastleHospital((r)=>{
				setTimeout(()=>{
					doWork(true);
				}, 3000);
			});
			return;
		}
		
		var items = cga.getInventoryItems();
		if(items.length >= 20){
			console.log('物品栏已满，制造结束');
			return;
		}
		
		var pl = cga.GetPlayerInfo();
		if(pl.punchclock > 0 && !pl.usingpunchclock){
			cga.travel.falan.toCastleClock((r)=>{
				cga.walkList([
				[41, 83, null],
				[41, 91, null],
				], ()=>{
					docraft(doWork);
				});
			});
		} else {
			docraft(doWork);
		}
	}
	
	doWork(true);	
});