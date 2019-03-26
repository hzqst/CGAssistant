var cga = require('./cgaapi')(function(){
	console.log('开始执行模块：法兰到营地');

	cga.travel.falan.toStone('S', function(r){
		if(!r){
			cb(r);
			return;
		}
		const walkToS1 = [
			[148, 156, null],
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
});