var cga = require('./cgaapi')(function(){
	const walkToZhzj = [
		[43, 82, null],
		[50, 83, null],
		[50, 89, null],
		[48, 89, null],
		[47, 85, '召唤之间'],
		[4, 15, null],
		[27, 17, null],
		[27, 8, '回廊'],
		[43, 6, null],
		[8, 6, null],
		[8, 27, null],
		[23, 27, null],
		[23, 19, '灵堂'],
		[30, 53, null],
		[20, 52, null],
		[21, 37, null],
		[9, 37, null],
		[8, 51, null],
		[7, 52, '地下牢房'],
		[24, 42, null],
		[25, 36, null],
		[30, 35, null],
		[30, 21, null],
		[31, 20, null],
	];

	var task = cga.task.Task('矿山钥匙', [
	{
		intro: '1.到法兰城里谢里雅堡(47.85)进入召唤之间，从召唤之间右上角进入回廊。由回廊(23.19)进入灵堂，再从灵堂(7.52)的楼梯进入地下牢房。在地下牢房的(31.20)跟 乔尔凯夫 交谈取得"给朋友的信"',
		workFunc: function(cb2){
			cga.travel.falan.toStone('C', function(r){
				if(!r){
					cb2(false);
					return;
				}
				cga.walkList(walkToZhzj, function(r){
					if(!r){
						cb2(false);
						return;
					}
					cga.TurnTo(33, 20);
					cga.AsyncWaitNPCDialog(function(dlg){					
						cga.ClickNPCDialog(32, -1);
						cga.AsyncWaitNPCDialog(function(dlg2){
							cga.ClickNPCDialog(4, -1);
							setTimeout(()=>{
								cb2(true);
							}, 1000);
						});
					});
				});
			});
		}
	},
	{
		intro: '2.到法兰城(61.63)进入仓库，把信交给 德米多夫 换取[矿山钥匙]',
		workFunc: function(cb2){
			cga.travel.falan.toStone('W1', function(r){
				if(!r){
					cb2(false);
					return;
				}
				const walkTo1 = [
					[60, 79, null],
					[60, 63, null],
					[61, 63, '仓库内部'],
					[11, 10, null],
				];

				cga.walkList(walkTo1, function(r){
					cga.TurnTo(12, 9);
					cga.AsyncWaitNPCDialog(function(dlg2){
						cga.ClickNPCDialog(4, -1);
						cga.AsyncWaitNPCDialog(function(dlg3){
							cga.ClickNPCDialog(1, -1);
							cga.AsyncWaitNPCDialog(function(dlg3){
								cga.ClickNPCDialog(1, -1);
								setTimeout(()=>{
									cb2(true);
								}, 1000);
							});
						});
					});
				});
			});
		}
	}
	],
	[//任务阶段是否完成
		function(){//gei peng you de xin
			return (cga.getItemCount('#18422') > 0) ? true : false;
		},
		function(){//矿山钥匙
			return (cga.getItemCount('矿山钥匙') > 0) ? true : false;
		},
	]		
	);
	
	task.doTask(()=>{
		console.log('ok');
	});
});