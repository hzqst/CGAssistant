//begin from 艾尔莎岛 医院

var cga = require('./cgaapi')(function(){
	console.log('高地鸡 起始地点：艾夏岛 医院 或 艾尔莎岛')
	
	cga.gaodiji = ()=>{
		cga.SayWords('高地鸡 开始...', 0, 3, 1);
		var stone_to_gaodi = (cb)=>{
			cga.travel.newisland.toStone('D', ()=>{
				const walkTo = [
					[150, 99, null],
					[161, 107, null],
					[176, 115, null],
					[189, 115, null],
					[190, 116, null],
					[204, 212, '盖雷布伦森林'],
					[212, 215, null],
					[213, 222, null],
					[215, 228, null],
					[222, 229, null],
					[225, 228, null],
					[230, 223, null],
					[231, 222, '布拉基姆高地'],
					[33, 189, null],
					[34, 188, null],
				];
				cga.walkList(walkTo, function(r){
					if(!r){
						cb(false);
						return;
					}
					cb(true);
				});
			});
		}
		
		var hosiptal_to_gaodi = (cb)=>{
			const walkTo = [
				[28, 51, null],
				[28, 52, '艾夏岛'],
				[112, 89, null],
				[121, 87, null],
				[130, 93, null],
				[140, 99, null],
				[150, 99, null],
				[161, 107, null],
				[176, 115, null],
				[189, 115, null],
				[190, 116, null],
				[204, 212, '盖雷布伦森林'],
				[212, 215, null],
				[213, 222, null],
				[215, 228, null],
				[222, 229, null],
				[225, 228, null],
				[230, 223, null],
				[231, 222, '布拉基姆高地'],
				[33, 189, null],
				[34, 188, null],
			];
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(false);
					return;
				}
				cb(true);
			});
		}
		
		var gaodi_to_hosiptal = (cb)=>{
			const walkTo = [
				[33, 189, null],
				[32, 191, null],
				[31, 192, null],
				[30, 193, '盖雷布伦森林'],
				[227, 226, null],
				[222, 229, null],
				[216, 229, null],
				[213, 225, null],
				[213, 220, null],
				[210, 214, null],
				[199, 211, '艾夏岛'],
				[190, 116, null],
				[189, 115, null],
				[176, 115, null],
				[161, 107, null],
				[150, 99, null],
				[140, 99, null],
				[137, 93, null],
				[128, 93, null],
				[122, 91, null],
				[116, 89, null],
				[112, 86, null],
				[112, 81, '医院'],
				[31, 48, null],
				[35, 45, null],
				[35, 46, null],
				[35, 45, null],
			];
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(false);
					return;
				}
				cb(true);
			});
		}
		
		var startBattle = ()=>{
			cga.freqMove(0, function(tt){
				var playerinfo = cga.GetPlayerInfo();
				if(playerinfo.mp < 100){
					clearInterval(tt);
					gaodi_to_hosiptal((err)=>{

						cga.TurnTo(36,46);
						cga.SayWords('开始补给...', 0, 3, 1);
						setTimeout(()=>{
							cga.SayWords('补给完毕!', 0, 3, 1);
							cga.gaodiji();
						}, 5000);
					});
				}
			});
		}
		var map = cga.GetMapName();
		if(map == '医院'){
			hosiptal_to_gaodi(()=>{
				startBattle();
			});
		} else if(map == '布拉基姆高地'){
			startBattle();
		} else {
			stone_to_gaodi(()=>{
				startBattle();
			});
		}
	}
	
	cga.gaodiji();
});