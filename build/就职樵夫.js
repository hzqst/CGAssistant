var cga = require('./cgaapi')(function(){
	const walkToJob = [
		[231, 78, null],
		[225, 70, null],
		[222, 69, null],
		[208, 67, null],
		[202, 66, null],
		[194, 58, null],
		[193, 52, null],
		[195, 50, '职业介绍所'],
		[7, 10, null]
	];

	const walkToCakeStore = [
		[230, 112, null],
		[214, 119, null],
		[214, 147, null],
		[216, 148, '艾文蛋糕店'],
		[5, 6, null],
		[11, 6, null]
	];

	const walkToFulun = [
		[130, 165, null],
		[116, 189, null],
		[107, 191, null]
	];
	
	var task = cga.task.Task('就职樵夫', [
	{
		intro: '1.到法兰城内职业介绍所[193.51]找募集樵夫的阿空交谈并学习伐木体验技能。',
		workFunc: function(cb2){
			cga.travel.falan.toStone('E2', function(r){
				if(!r){
					cb2(false);
					return;
				}
				cga.walkList(walkToJob, function(r){
					if(!r){
						cb2(false);
						return;
					}
					cga.TurnTo(8, 11);
					cga.AsyncWaitNPCDialog(function(dlg){
						cga.ClickNPCDialog(0, 0);
						cga.AsyncWaitNPCDialog(function(dlg2){
							cga.ClickNPCDialog(0, -1);
							cga.AsyncWaitNPCDialog(function(dlg3){
								var skill = cga.findPlayerSkill('伐木体验');
								if(!skill){
									throw new Error('伐木体验 学习失败');
									return;
								}
								cb2(true);
							});
						});
					});
				});
			});
		}
	},
	{
		intro: '2.学到技能后再到法兰城外的树旁使用伐木体验技能伐下20个孟宗竹。',
		workFunc: function(cb2){
			cga.travel.falan.toStone('E', function(r){
				if(!r){
					cb2(false);
					return;
				}
				const walkToCutWood_1 = [
					[244, 96, null],
					[247, 89, null],
					[280, 88, null],
					[281, 88, '芙蕾雅']
				];
				const walkToCutWood_2 = [
					[239, 81, null],
					[248, 87, null],
					[280, 88, null],
					[281, 88, '芙蕾雅']
				];
				var xy = cga.GetMapXY();
				var stone = cga.travel.falan.xy2name(xy.x, xy.y);
				cga.walkList((stone && stone.charAt(1) == '1') ? walkToCutWood_1 : walkToCutWood_2, function(r){
					var skill = cga.findPlayerSkill('伐木体验');
					if(!skill){
						cb2(false);
						return;
					}
					var timer2 = cga.keepStartWork(skill.index, 0);
					//TODO
					var waitEnd = function(cb2){
						cga.AsyncWaitWorkingResult(function(r){
							var playerInfo = cga.GetPlayerInfo();
							if(playerInfo.mp == 0)
							{
								stats.printStats();
								cb2(true, '蓝量耗尽，采集结束');
								return;
							}								
							if(cga.getItemCount('孟宗竹') >= 20)
							{
								stats.printStats();
								cb2(true, '孟宗竹x20 get，采集结束');
								return;
							}
							var items = cga.getInventoryItems();
							for(var i in items){
								if(items[i].name == '印度轻木' && items[i].count == 40){
									cga.DropItem(items[i].pos);
									break;
								}
							}
							waitEnd(cb2);
						}, 10000);
					}
					waitEnd(cb2);
				});
			});
		}
	},
	{
		intro: '3.带着伐下的孟宗竹回到法兰城内艾文蛋糕店[216.148]内找艾文交谈后就可以换到手斧。',
		workFunc: function(cb2){
			cga.travel.falan.toStone('E1', function(r){
				if(!r){
					cb2(false);
					return;
				}
				cga.walkList(walkToCakeStore, function(r){
					if(!r){
						cb2(false);
						return;
					}
					cga.TurnTo(12, 5);
					cga.AsyncWaitNPCDialog(function(dlg){
						cga.ClickNPCDialog(32, -1);
						cga.AsyncWaitNPCDialog(function(dlg2){
							cga.ClickNPCDialog(4, -1);
							cga.AsyncWaitNPCDialog(function(dlg3){
								cb2(true);
							});
						});
					});
				});
			});
		}
	},
	{
		intro: '4.然后再到城内[106.190]的地点找樵夫弗伦（凌晨及白天出现）换取树苗。',
		workFunc: function(cb2){
			cga.travel.falan.toStone('S1', function(r){
				if(!r){
					cb2(false);
					return;
				}

				cga.walkList(walkToFulun, function(r){
					if(!r){
						cb2(false);
						return;
					}

					cga.task.waitForNPC('樵夫弗伦', function(){
						cga.TurnTo(106, 191);
						cga.AsyncWaitNPCDialog(function(dlg){
							if(dlg.options == 12){
								cga.ClickNPCDialog(4, -1);
								cga.AsyncWaitNPCDialog(function(dlg2){
									cb2(true);
								});
							} else {
								cb2(false);
							}
						});
					});
				});
			});
		}
	},
	{
		intro: '5.利用白天时再把树苗交给种植家阿姆罗斯（134，36） 交给他之后就可以换取到水色的花。',
		workFunc: function(cb2){
			cga.travel.falan.toStone('S1', function(r){
				if(!r){
					cb2(false);
					return;
				}
				const walkToAmuros = [
					[139, 148, null],
					[126, 133, null],
					[126, 122, null],
					[126, 52, null],
					[134, 44, null],
					[134, 37, null]
				];
				cga.walkList(walkToAmuros, function(r){
					if(!r){
						cb2(false);
						return;
					}

					cga.task.waitForNPC('种树的阿姆罗斯', function(){
						cga.TurnTo(134, 36);
						cga.AsyncWaitNPCDialog(function(dlg){
							if(dlg.options == 12){
								cga.ClickNPCDialog(4, -1);
								cga.AsyncWaitNPCDialog(function(dlg2){
									cb2(true);
								});
							} else {
								cb2(false);
							}
						});
					});
				});
			});
		}
	},
	{
		intro: '6.换到花之后再把他交给弗伦后就可以再换到木柴。 ',
		workFunc: function(cb2){
			cga.travel.falan.toStone('S1', function(r){
				if(!r){
					cb2(false);
					return;
				}
				cga.walkList(walkToFulun, function(r){
					if(!r){
						cb2(false);
						return;
					}
					cga.task.waitForNPC('樵夫弗伦', function(){
						cga.TurnTo(106, 191);
						cga.AsyncWaitNPCDialog(function(dlg){
							if(dlg.options == 12){
								cga.ClickNPCDialog(4, -1);
								cga.AsyncWaitNPCDialog(function(dlg2){
									cb2(true);
								});
							} else {
								cb2(false);
							}
						});
					});					
				});
			});
		}
	},
	{
		intro: '7.拿着换到的木柴再回到法兰城的艾文蛋糕店[216.148]内找艾文交谈之后就可以换到点心。',
		workFunc: function(cb2){
			cga.travel.falan.toStone('E1', function(r){
				if(!r){
					cb2(false);
					return;
				}
				cga.walkList(walkToCakeStore, function(r){
					if(!r){
						cb2(false);
						return;
					}
					cga.TurnTo(12, 5);
					cga.AsyncWaitNPCDialog(function(dlg){
						cb2(true);
					});
				});
			});
		}
	},
	{
		intro: '8.再把点心拿给弗伦后就可以换到樵夫推荐信。',
		workFunc: function(cb2){
			cga.travel.falan.toStone('S1', function(r){
				if(!r){
					cb2(false);
					return;
				}
				cga.walkList(walkToFulun, function(r){
					if(!r){
						cb2(false);
						return;
					}
					cga.task.waitForNPC('樵夫弗伦', function(){
						cga.TurnTo(106, 191);
						cga.AsyncWaitNPCDialog(function(dlg){
							cb2(true);
						});
					});
				});
			});
		}
	},
	{
		intro: '9.最后再回到职业介绍所[193.51]内找樵夫荷拉巴斯说话后就可以就职。',
		workFunc: function(cb2){
			cga.travel.falan.toStone('E2', function(r){
				if(!r){
					cb2(false);
					return;
				}
				cga.walkList(walkToJob, function(r){
					if(!r){
						cb2(false);
						return;
					}
					cga.TurnTo(7, 11);
					cga.AsyncWaitNPCDialog(function(dlg){
						cga.ClickNPCDialog(0, 0);							
						cga.AsyncWaitNPCDialog(function(dlg2){
							cb2(true);
						});
					});
				});
			});
		}
	}
	],
	[//任务阶段是否完成
		function(){//伐木体验
			return (!cga.findPlayerSkill('伐木体验')) ? true : false;
		},
		function(){//孟宗竹>=20
			return (cga.getItemCount('孟宗竹') >= 20) ? true : false;
		},
		function(){//手斧
			return (cga.getItemCount('#18179') > 0) ? true : false;
		},
		function(){//树苗
			return (cga.getItemCount('#18180') > 0) ? true : false;
		},
		function(){//水色的花
			return (cga.getItemCount('#18181') > 0) ? true : false;
		},
		function(){//木材
			return (cga.getItemCount('#18178') > 0) ? true : false;
		},
		function(){//艾文的饼干
			return (cga.getItemCount('#18212') > 0) ? true : false;
		},
		function(){//樵夫推荐信
			return (cga.getItemCount('樵夫推荐信') > 0) ? true : false;
		}
	]		
	);
	
	task.doTask(()=>{
		console.log('ok');
	});
});