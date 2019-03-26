var cga = require('bindings')('node_cga');	
var moment = require('moment');

module.exports = function(callback){
	var port = 4396;
	if(process.argv.length >= 3)
		port = process.argv[2];
	cga.AsyncConnect(port, function(result){
		if(!result){
			throw new Error('Failed to connect to game.');
			return;
		}
		callback();
	});
	
	cga.isInBattle = function(){
		return (cga.GetWorldStatus() == 10) ? true : false;
	}
	
	cga.keepStartWork = function(skill_index, subskill_index){
		var started = cga.StartWork(skill_index, subskill_index);
		return setInterval(function(){
			if(cga.GetGameStatus() != 3 || cga.GetWorldStatus() != 9)
				started = false;
			if(!started)
			{
				if(cga.GetGameStatus() == 3 && cga.GetWorldStatus() == 9)
					started = cga.StartWork(skill_index, subskill_index);
			}
		}, 100);
	}

	cga.getItemCraftInfo = function(itemname){
		var skills = cga.GetSkillsInfo();
		for(var i in skills){
			if(skills[i].type == 1){
				var crafts = cga.GetCraftsInfo(skills[i].index);
				for(var j in crafts){
					if(crafts[j].name == itemname){
						return {skill : skills[i], craft : crafts[j]};
					}
				}
			}
		}
		return null;
	}

	//鉴定物品，参数：物品位置
	//返回：true / false
	cga.assessItem = function(itempos){
		var skill = cga.findPlayerSkill('鉴定');
		if(skill){
			if(cga.StartWork(skill.index, 0))
				return cga.AssessItem(skill.index, itempos);
		}
		return false;		
	}
	
	//制造物品，参数：物品名
	cga.craftNamedItem = function(itemname){
		var info = cga.getItemCraftInfo(itemname);
		if(!info)
			return new Error('你没有制造 '+itemname+' 的技能');
		
		var items = cga.getInventoryItems();
		var arr = new Array();
		for(var j in info.craft.materials){
			var found = false;
			for(var i in items){
				if(items[i].itemid == info.craft.materials[j].itemid &&
				items[i].count >= info.craft.materials[j].count){
					arr.push(items[i].pos);
					found = true;
					break;
				}
			}
			if(!found){
				return new Error('制造' +itemname+' 所需物品 ' +info.craft.materials[j].name+'不足！');
			}
		}
		console.log(info.skill.index);
		console.log(info.craft.index);
		console.log(arr);
		cga.StartWork(info.skill.index, info.craft.index);
		cga.CraftItem(info.skill.index, info.craft.index, 0, arr);
		return true;
	}

	cga.getInventoryItems = function(){
		var items = cga.GetItemsInfo();
		var newitems = new Array();
		for(var i in items)
		{
			if(items[i].pos >= 8)
				newitems.push(items[i]);
		}
		return newitems;
	}
	
	cga.useItemForMyPet = function(itempos){
		var player = cga.GetPlayerInfo();
		var pet = cga.GetPetInfo(player.petid);
		cga.AsyncUseItemFor(itempos, function(players){
			if(players == false){
				console.log('AsyncUseItemFor failed');
				return;
			}
			for(var i in players){
				if(players[i].name == player.name) {
					cga.AsyncUseItemSelectPlayer(i, function(units) {
						if(units == false) {
							console.log('AsyncUseItemSelectPlayer failed');
							return;
						}
						for(var i in units) {
							if(units[i].name == pet.name) {
								cga.UseItemSelectUnit(units[i].index);
								return;
							}
						}
						console.log('AsyncUseItemSelectPlayer pet not found');
					});
					return;
				}
			}
			console.log('AsyncUseItemFor player not found');
		});
	}
		
	//参数： 方向, 0=右上，2=右下, 4=左下，6=左上
	cga.turnDir = function(dir){
		var xy = cga.GetMapXY();
		var x = xy.x;
		var y = xy.y;
		switch(dir){
			case 0: cga.TurnTo(x+2, y);break;
			case 1: cga.TurnTo(x+2, y+2);break;
			case 2: cga.TurnTo(x, y+2);break;
			case 3: cga.TurnTo(x-2, y+2);break;
			case 4: cga.TurnTo(x-2, y);break;
			case 5: cga.TurnTo(x-2, y-2);break;
			case 6: cga.TurnTo(x, y-2);break;
			case 7: cga.TurnTo(x+2, y-2);break;
			default: throw new Error('Invalid direction');
		}
	}
	
	cga.travel = {};
	
	cga.travel.falan = {};
	
	cga.travel.falan.xy2name = function(x, y){
		if(x == 242 && y == 100)
			return 'E1';
		if(x == 141 && y == 148)
			return 'S1';
		if(x == 63 && y == 79)
			return 'W1';
		if(x == 233 && y == 78)
			return 'E2';
		if(x == 162 && y == 130)
			return 'S2';
		if(x == 72 && y == 123)
			return 'W2';
		return false;
	}
	
	cga.travel.falan.isvalid = function(stone){
		switch(stone.toUpperCase()){
			case 'E': return true;
			case 'S': return true;
			case 'W': return true;
			case 'E1': return true;
			case 'S1': return true;
			case 'W1': return true;
			case 'E2': return true;
			case 'S2': return true;
			case 'W2': return true;
			case 'M1': return true;//市场
			case 'M3': return true;
			case 'C': return true;//里谢里雅堡
		}
		return false;
	}
	
	cga.travel.falan.name2xy = function(stone){
		switch(stone.toUpperCase()){
			case 'E1': return [242, 100];
			break;
			case 'S1': return [141, 148];
			break;
			case 'W1': return [63, 79];
			break;
			case 'E2': return [233, 78];
			break;
			case 'S2': return [162, 130]
			break;
			case 'W2': return [72, 123];
			break;
			case 'M3': return [46, 16];
			break;	
			case 'M1': return [46, 16];
			break;
		}
		return false;
	}

	cga.travel.falan.toStoneInternal = function(stone, cb, r){
		if(!r){
			cb(false);
			return;
		}
		var curXY = cga.GetMapXY();
		var curMap = cga.GetMapName();
		const desiredMap = ['法兰城','里谢里雅堡','艾尔莎岛','市场一楼 - 宠物交易区','市场三楼 - 修理专区'];
		if(curMap == '法兰城'){
			if(stone == 'C'){
				cga.travel.falan.toCastle(cb);
				return;
			}
			
			var curStone = cga.travel.falan.xy2name(curXY.x, curXY.y);
			if(curStone) {
				var turn = false;
				if(stone.length >= 2 && curStone.charAt(1) == stone.charAt(1)) {
					if(curStone == stone){
						cb(true);
						return;
					}
					turn = true;
				} else if(stone.length < 2){
					if(curStone.charAt(0) == stone.charAt(0)){
						cb(true);
						return;
					}
					turn = true;
				}
				if(turn){
					switch(curStone){
						case 'E2':cga.turnDir(6);break;
						case 'S2':cga.turnDir(0);break;
						case 'W2':cga.turnDir(0);break;
						case 'E1':cga.turnDir(0);break;
						case 'S1':cga.turnDir(6);break;
						case 'W1':cga.turnDir(6);break;
					}
					cga.AsyncWaitMovement({map:desiredMap, delay:1000, timeout:5000}, function(r){
						cga.travel.falan.toStoneInternal(stone, cb, r);
					});
					return;
				}
			}
		}
		
		if(curMap.indexOf('市场') >= 0 && curXY.x == 46 && curXY.y == 16){
			cga.turnDir(6);
			cga.AsyncWaitMovement({map:desiredMap, delay:1000, timeout:5000}, function(r){
				cga.travel.falan.toStoneInternal(stone, cb, r);
			});
			return;
		}
		if(curMap == '艾尔莎岛' && curXY.x == 140 && curXY.y == 105){
			cga.turnDir(7);
			cga.AsyncWaitNPCDialog(function(dlg){
				cga.ClickNPCDialog(4, -1);
				cga.AsyncWaitMovement({map:desiredMap, delay:1000, timeout:5000}, function(r){
					cga.travel.falan.toStoneInternal(stone, cb, r);
				});
			});
			return;
		}
		if(curMap == '里谢里雅堡' && curXY.x == 41 && curXY.y == 91){
			if(stone == 'C'){
				cb(true);
				return;
			}
			var walks = null;
			const walkOutOfCastle_1 = [
				[41, 98, '法兰城'],
				[153, 122, null],
				[143, 148, null],
				[141, 148, null]
			];
			const walkOutOfCastle_2 = [
				[40, 98, '法兰城'],
				[153, 122, null],
				[162, 130, null]
			];
			if(stone.length == 1)
				walks = walkOutOfCastle_2;
			else if(stone.length >= 2 && stone.charAt(1) == '1')
				walks = walkOutOfCastle_1; 
			else
				walks = walkOutOfCastle_2;

			cga.walkList(walks, function(r){
				if(!r){
					cb(false);
					return;
				}
				cga.travel.falan.toStoneInternal(stone, cb, true);
			});
			return;
		}
		if(curMap == '里谢里雅堡' && curXY.x >= 34 && curXY.x <= 44 && curXY.y >= 79 && curXY.y <= 90 && stone == 'C'){
			cga.walkList([
			[33, 90, null],
			[29, 91, null],
			[29, 93, null],
			[41, 93, null],
			[41, 91, null],
			]
			, function(r){
				if(!r){
					cb(false);
					return;
				}
				cga.travel.falan.toStoneInternal(stone, cb, true);
			});
			return;
		}
		if(curMap == '里谢里雅堡' && curXY.x == 27 && curXY.y == 82){
			if(stone == 'C'){
				cb(true);
				return;
			}
			var walks = null;
			const walkOutOfCastle_1 = [
				[37, 84, null],
				[40, 97, null],
				[40, 98, '法兰城'],
				[153, 122, null],
				[143, 148, null],
				[141, 148, null]
			];
			const walkOutOfCastle_2 = [
				[37, 84, null],
				[40, 97, null],
				[40, 98, '法兰城'],
				[153, 122, null],
				[162, 130, null]
			];
			if(stone.length == 1)
				walks = walkOutOfCastle_2;
			else if(stone.length >= 2 && stone.charAt(1) == '1')
				walks = walkOutOfCastle_1;
			else
				walks = walkOutOfCastle_2;

			cga.walkList(walks, function(r){
				if(!r){
					cb(false);
					return;
				}
				cga.travel.falan.toStoneInternal(stone, cb, true);
			});
			return;
		}
		//重新回城
		cga.LogBack();
		cga.AsyncWaitMovement({map:desiredMap, delay:1000, timeout:5000}, function(r){
			cga.travel.falan.toStoneInternal(stone, cb, r);
		});
	}
	
	//参数1：传送石名称，有效参数：E1 S1 W1 E2 S2 W2 M1(道具-市场1楼) M3(道具-市场3楼)
	//参数2：回调函数function(result), result 为true或false
	cga.travel.falan.toStone = function(stone, cb){
		if(!cga.travel.falan.isvalid(stone)){
			cb(false);
			return;
		}
		
		cga.travel.falan.toStoneInternal(stone, cb, true);
	}
		
	//前往到法兰城东医院
	//参数1：回调函数function(result), result 为true或false
	cga.travel.falan.toEastHospital = function(cb){
		cga.travel.falan.toStone('E', function(r){
			if(!r){
				cb(false);
				return;
			}
						
			const walkToHospital_1 = [
				[221, 84, null],
				[221, 83, '医院']
			];
			const walkToHospital_2 = [
				[230, 79, null],
				[221, 84, null],
				[221, 83, '医院']
			];
			
			var xy = cga.GetMapXY();
			var stone = cga.travel.falan.xy2name(xy.x, xy.y);
			cga.walkList(stone.charAt(1) == '1' ? walkToHospital_1 : walkToHospital_2, function(r){
				if(!r){
					cb(false);
					return;
				}
				cb(true);
			});
		});
	}
	
	//前往到法兰城西医院
	//参数1：回调函数function(result), result 为true或false
	cga.travel.falan.toWestHospital = function(cb){
		cga.travel.falan.toStone('W', function(r){
			if(!r){
				cb(false);
				return;
			}

			const walkToHospital_1 = [
				[67, 84, null],
				[82, 84, null],
				[82, 83, '医院']
			];
			const walkToHospital_2 = [
				[75, 107, null],
				[82, 84, null],
				[82, 83, '医院']
			];
			
			var xy = cga.GetMapXY();
			var stone = cga.travel.falan.xy2name(xy.x, xy.y);
			cga.walkList(stone.charAt(1) == '1' ? walkToHospital_1 : walkToHospital_2, function(r){
				if(!r){
					cb(false);
					return;
				}
				cb(true);
			});
		});
	}
	
	//前往到法兰城银行
	//参数1：回调函数function(result), result 为true或false
	cga.travel.falan.toBank = function(cb){
		
		if(cga.GetMapName() == '银行'){
			const walkTo = [
				[7, 11, null],
				[11, 8, null],
			];
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(false);
					return;
				}
				cga.TurnTo(12, 8);
				cb(true);
			});
			return;
		}
		
		cga.travel.falan.toStone('E', function(r){
			if(!r){
				cb(false);
				return;
			}

			const walkTo1 = [
				[235, 107, null],
				[235, 110, null],
				[238, 111, '银行'],
				[7, 11, null],
				[11, 8, null],
			];
			const walkTo2 = [
				[236, 78, null],
				[236, 91, null],
				[235, 110, null],
				[238, 111, '银行'],
				[7, 11, null],
				[11, 8, null],
			];
			
			var xy = cga.GetMapXY();
			var stone = cga.travel.falan.xy2name(xy.x, xy.y);
			cga.walkList(stone.charAt(1) == '1' ? walkTo1 : walkTo2, function(r){
				if(!r){
					cb(false);
					return;
				}
				cga.TurnTo(12, 8);
				cb(true);
			});
		});
	}
	
	//从法兰城到里谢里雅堡，启动地点：登出到法兰城即可
	//参数1：回调函数function(result), result 为true或false
	cga.travel.falan.toCastle = function(cb){
		cga.travel.falan.toStone('S', function(r){
			if(!r){
				cb(false);
				return;
			}

			const walkToCastle_1 = [
				[143, 148, null],
				[151, 122, null],
				[152, 101, null],
				[152, 100, '里谢里雅堡']
			];
			const walkToCastle_2 = [
				[158, 128, null],
				[155, 122, null],
				[154, 101, null],
				[154, 100, '里谢里雅堡']
			];
			var xy = cga.GetMapXY();
			var stone = cga.travel.falan.xy2name(xy.x, xy.y);
			cga.walkList(stone.charAt(1) == '1' ? walkToCastle_1 : walkToCastle_2, function(r){
				if(!r){
					cb(false);
					return;
				}
				cb(true);
			});
		});
	}
	
	cga.travel.falan.toCastleHospital = function(cb){
		cga.travel.falan.toStone('C', (r)=>{
			if(!r){
				cb(false);
				return;
			}
			var xy = cga.GetMapXY();
			var walkTo = null;
			if(xy.x == 27 && xy.y == 82)
			{
				walkTo = [
					[31, 90, null],
					[33, 90, null],
					[34, 89, null],
				];
			} else {
				walkTo = [
					[36, 93, null],
					[29, 93, null],
					[29, 91, null],
					[32, 91, null],
					[34, 89, null],
				];
			}
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(false);
					return;
				}
				cga.TurnTo(36, 87);
				cb(true);
			});	
		});	
	}
	
	cga.travel.falan.toCastleClock = function(cb){
		cga.travel.falan.toStone('C', (r)=>{
			if(!r){
				cb(false);
				return;
			}
			var xy = cga.GetMapXY();
			var walkTo = null;
			if(xy.x == 27 && xy.y == 82)
			{
				walkTo = [
					[31, 83, null],
					[41, 83, null],
					[58, 83, null],
				];
			} else {
				walkTo = [
					[41, 83, null],
					[58, 83, null],
				];
			}
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(false);
					return;
				}
				cga.TurnTo(58, 84);
				cga.AsyncWaitNPCDialog(function(dlg){

					if(dlg.options == 12){
						cga.ClickNPCDialog(4, -1);
						cga.AsyncWaitNPCDialog(function(dlg2){
							cb(true);
						});
					} else {
						cb(true);
					}
				});
			});
		});	
	}

	cga.travel.falan.toFabricStore = (cb)=>{
		if(cga.GetMapName()=='流行商店'){
			const walkToStore = [
				[8, 7, null],
			];
			cga.walkList(walkToStore, function(r){
				if(!r){
					cb(false);
					return;
				}
				cb(true);
			});
			return;
		}
		
		cga.travel.falan.toStone('S1', function(r){
			const walkToStore = [
				[135, 146, null],
				[124, 140, null],
				[114, 134, null],
				[114, 115, null],
				[116, 112, null],
				[117, 112, '流行商店'],
				[8, 7, null],
			];
			cga.walkList(walkToStore, function(r){
				if(!r){
					cb(false);
					return;
				}
				cb(true);
			});
		});
	}
	
	cga.travel.falan.toMineStore = (mine, cb)=>{
		var mineExchange = null;
		
		if(mine == '银'){
			mineExchange = (cb2)=>{
				cga.walkList([[25, 7, null], [29, 6, null]], function(r){
					cga.TurnTo(30, 5);
					cb2(true);
				});
			}
		}
		if(mine == '铁'){
			mineExchange = (cb2)=>{
				cga.walkList([[25, 7, null], [28, 6, null]], function(r){
					cga.TurnTo(28, 5);
					cb2(true);
				});
			}
		}
		
		if(cga.GetMapName()=='米克尔工房'){
			const walkToStore = [
				[24, 17, null],
				[25, 12, null],
			];
			cga.walkList(walkToStore, function(r){
				if(!r){
					cb(false);
					return;
				}
				if(mineExchange){
					mineExchange(()=>{
						cb(true);
					});
				}else{
					cb(true);
				}
			});
			return;
		}
		
		cga.travel.falan.toStone('W1', function(r){
			const walkToStore = [
				[67, 74, null],
				[77, 68, null],
				[100, 64, null],
				[100, 61, '米克尔工房'],
				[8, 20, null],
				[12, 18, null],
				[24, 17, null],
				[25, 12, null],
			];
			cga.walkList(walkToStore, function(r){
				if(!r){
					cb(false);
					return;
				}
				
				if(mineExchange){
					mineExchange(()=>{
						cb(true);
					});
				}else{
					cb(true);
				}
			});
		});
	}

	//从法兰城到新城，启动地点：登出到法兰城即可
	//参数1：回调函数function(result), result 为true或false
	cga.travel.falan.toNewIsland = function(cb){
		cga.travel.falan.toStone('C', function(r){
			if(!r){
				cb(false);
				return;
			}
			const walkTo = [
				[36, 93, null],
				[29, 93, null],
				[28, 88, null]
			];
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(false);
					return;
				}
				cga.AsyncWaitNPCDialog(function(dlg){
					cga.ClickNPCDialog(32, -1);
					cga.AsyncWaitNPCDialog(function(dlg2){
						cga.ClickNPCDialog(32, -1);
						cga.AsyncWaitNPCDialog(function(dlg3){
							cga.ClickNPCDialog(32, -1);
							cga.AsyncWaitNPCDialog(function(dlg4){
								cga.ClickNPCDialog(32, -1);
								cga.AsyncWaitNPCDialog(function(dlg5){
									if(dlg5.options == 12){											
										cga.ClickNPCDialog(4, -1);
										cga.AsyncWaitMovement({map:'？'}, function(r){
											const walkTo_1 = [
												[19, 21, '法兰城遗迹'],
												[99, 51, null],
												[117, 71, null],
												[119, 78, null],
												[119, 86, null],
												[100, 90, null],
												[98, 93, null],
												[96, 137, null],
												[96, 138, '盖雷布伦森林'],
												[116, 148, null],
												[124, 160, null],
												[124, 168, '温迪尔平原'],
												[234, 80, null],
												[242, 88, null],
												[250, 107, null],
												[263, 108, null],
												[264, 108, '艾尔莎岛'],
												[130, 105, null],
												[141, 105, null]
											];
														
											cga.walkList(walkTo_1, function(r){
												if(!r){
													cb(false);
													return;
												}
												cb(true);
											});
										});
									} else {
										cb(false);
									}
								});
							});
						});
					});
				});	
			});
		});	
	}
		
	//前往启程之间
	//参数1：回调函数function(result), result 为true或false
	cga.travel.falan.toTeleRoom = function(cb){
		cga.travel.falan.toStone('C', function(r){
			if(!r){
				cb(false);
				return;
			}
			const walkTo = [
				[41, 70, null],
				[41, 51, null],
				[41, 50, '里谢里雅堡 1楼'],
				[71, 32, null],
				[64, 24, null],
				[49, 24, null],
				[45, 20, '启程之间'],
				[25, 27, null]
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
	
	//前往亚留特村
	//参数1：回调函数function(result), result 为true或false
	cga.travel.falan.toYaliute = function(cb){
		cga.travel.falan.toTeleRoom(function(r){
			if(!r){
				cb(r);
				return;
			}
			const walkTo = [
				[30, 27, null],
				[36, 23, null],
				[44, 23, null]
			];
			
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(r);
					return;
				}
				cga.TurnTo(44,20);
				cga.AsyncWaitNPCDialog(function(dlg){
					if(dlg.message.indexOf('费用是') == -1){
						cb(false, new Error('无法使用前往亚留特村的传送石'));
						return;
					}
					cga.ClickNPCDialog(4, -1);
					cga.AsyncWaitMovement({map:'亚留特村的传送点', delay:1000, timeout:5000}, function(r){
						cb(r);
						return;
					});
				});
			});
		});
	}
	
	//去伊尔村
	cga.travel.falan.toYiErCun = function(cb){
		cga.travel.falan.toTeleRoom(function(r){
			if(!r){
				cb(r);
				return;
			}
			const walkTo = [
				[27, 33, null],
				[37, 33, null],
				[44, 33, null]
			];
			
			cga.walkList(walkTo, function(r){
				if(!r){
					cb(r);
					return;
				}
				cga.TurnTo(44,32);
				cga.AsyncWaitNPCDialog(function(dlg){
					if(dlg.message.indexOf('个金币') == -1){
						cb(false, new Error('无法使用前往伊尔村的传送石'));
						return;
					}
					cga.ClickNPCDialog(4, -1);
					cga.AsyncWaitMovement({map:'伊尔村的传送点', delay:1000, timeout:5000}, function(r){
						cb(r);
						return;
					});
				});
			});
		});
	}
		
	//前往传送石。出发地：任何地方。
	//参数1：传送石所在城市名，如：法兰城
	//参数2：传送石名称
	//参数3：回调函数function(result), result 为true或false
	cga.travel.toStone = function(city, stone, cb){
		switch(city){
			case '法兰城':
				cga.travel.falan.toStone(stone, cb);
				return;
			case '法兰城到新城':
				cga.travel.falan.toNewIsland(cb);
				return;
		}
		cb(false);
	}
	
	//前往该城市。出发地：任何地方。
	//参数1：城市名，如：艾尔莎岛
	//参数2：回调函数function(result), result 为true或false
	cga.travel.toCity = function(city, cb){
		switch(city){
			case '新城':case '艾尔莎岛':
				cga.travel.falan.toNewIsland(cb);
				return;
		}
		cb(false);
	}
	
	cga.travel.newisland = {};
		
	cga.travel.newisland.isSettled = true;
	
	cga.travel.newisland.xy2name = function(x, y){
		if(x == 140 && y == 105)
			return 'X';
		if(x == 158 && y == 94)
			return 'A';
		if(x == 84 && y == 112)
			return 'B';
		if(x == 164 && y == 159)
			return 'C';
		if(x == 151 && y == 97)
			return 'D';
		return false;
	}
	
	cga.travel.newisland.isvalid = function(stone){
		switch(stone.toUpperCase()){
			case 'A': return true;
			case 'B': return true;
			case 'C': return true;
			case 'D': return true;
			case 'X': return true;
		}
		return false;
	}
	
	cga.travel.newisland.name2xy = function(stone){
		switch(stone.toUpperCase()){
			case 'X': return [140, 105];
			break;
			case 'A': return [158, 94];
			break;
			case 'B': return [84, 112];
			break;
			case 'C': return [164, 159];
			break;
			case 'D': return [151, 97];
			break;
		}
		return false;
	}
	
	cga.travel.newisland.toStoneInternal = (stone, cb, r)=>{
		if(!r){
			cb(false);
			return;
		}
		var curXY = cga.GetMapXY();
		var curMap = cga.GetMapName();
		const desiredMap = ['艾尔莎岛', '艾夏岛'];
		if(curMap == '艾尔莎岛' || curMap == '艾夏岛'){
			
			var curStone = cga.travel.newisland.xy2name(curXY.x, curXY.y);
			if(curStone) {
				console.log(curStone);
				var turn = false;
				if(stone.length >= 2 && curStone.charAt(1) == stone.charAt(1)) {
					if(curStone == stone){
						cb(true);
						return;
					}
					turn = true;
				} else if(stone.length < 2){
					if(curStone.charAt(0) == stone.charAt(0)){
						cb(true);
						return;
					}
					turn = true;
				}
				if(turn){
					switch(curStone){
						case 'X':{
							cga.walkList([
							[146, 104, null],
							[156, 94, null],
							[158, 94, null],
							], ()=>{
								cga.travel.newisland.toStoneInternal(stone, cb, r);
							});
							return;
						}
						case 'A':cga.turnDir(6);break;
						case 'B':cga.turnDir(4);break;
						case 'C':cga.turnDir(5);break;
						case 'D':cga.turnDir(6);break;
					}
					cga.AsyncWaitMovement({map:desiredMap, delay:1000, timeout:5000}, function(r){
						cga.travel.newisland.toStoneInternal(stone, cb, r);
					});
					return;
				}
			}
		}

		if(cga.travel.newisland.isSettled){
			cga.LogBack();
			cga.AsyncWaitMovement({map:desiredMap, delay:1000, timeout:5000}, function(r){
				cga.travel.newisland.toStoneInternal(stone, cb, r);
			});
		}
	}
	
	//参数1：传送石名称，有效参数：A B C D
	//参数2：回调函数function(result), result 为true或false
	cga.travel.newisland.toStone = function(stone, cb){
		if(!cga.travel.newisland.isvalid(stone)){
			cb(false);
			return;
		}
		
		cga.travel.newisland.toStoneInternal(stone, cb, true);
	}
	
	cga.travel.gelaer = {};
	
	//定居？
	cga.travel.gelaer.isSettled = true;
	
	cga.travel.gelaer.xy2name = function(x, y){
		if(x == 120 && y == 107)
			return 'N';
		if(x == 118 && y == 214)
			return 'S';
		return false;
	}
	
	cga.travel.gelaer.isvalid = function(stone){
		switch(stone.toUpperCase()){
			case 'N': return true;
			case 'S': return true;
		}
		return false;
	}
	
	cga.travel.gelaer.name2xy = function(stone){
		switch(stone.toUpperCase()){
			case 'N': return [120, 107];
			break;
			case 'S': return [118, 214];
			break;
		}
		return false;
	}
	
	cga.travel.gelaer.toStoneInternal = function(stone, cb, r){
		if(!r){
			cb(false);
			return;
		}
		var curXY = cga.GetMapXY();
		var curMap = cga.GetMapName();
		const desiredMap = ['哥拉尔镇'];
		if(curMap == '哥拉尔镇'){
			
			var curStone = cga.travel.gelaer.xy2name(curXY.x, curXY.y);
			if(curStone) {
				console.log(curStone);
				var turn = false;
				if(stone.length >= 2 && curStone.charAt(1) == stone.charAt(1)) {
					if(curStone == stone){
						cb(true);
						return;
					}
					turn = true;
				} else if(stone.length < 2){
					if(curStone.charAt(0) == stone.charAt(0)){
						cb(true);
						return;
					}
					turn = true;
				}
				if(turn){
					switch(curStone){
						case 'N':cga.turnDir(6);break;
						case 'S':cga.turnDir(0);break;
					}
					cga.AsyncWaitMovement({map:desiredMap, delay:1000, timeout:5000}, function(r){
						cga.travel.gelaer.toStoneInternal(stone, cb, r);
					});
					return;
				}
			}
		}

		if(cga.travel.gelaer.isSettled){
			cga.LogBack();
			cga.AsyncWaitMovement({map:desiredMap, delay:1000, timeout:5000}, function(r){
				cga.travel.gelaer.toStoneInternal(stone, cb, r);
			});
		}
	}
	
	//参数1：传送石名称，有效参数：N S
	//参数2：回调函数function(result), result 为true或false
	cga.travel.gelaer.toStone = function(stone, cb){
		if(!cga.travel.gelaer.isvalid(stone)){
			cb(false);
			return;
		}
		
		cga.travel.gelaer.toStoneInternal(stone, cb, true);
	}
	
	//前往到法兰城西医院
	//参数1：回调函数function(result), result 为true或false
	cga.travel.gelaer.toHospital = function(cb){
		cga.travel.gelaer.toStone('N', function(r){
			if(!r){
				cb(false);
				return;
			}

			const walkToHospital_1 = [
				[125, 105, null],
				[155, 105, null],
				[159, 100, null],
				[161, 92, null],
				[162, 91, null],
				[165, 91, '医院']
				[17, 26, null],
				[29, 27, null],
			];

			cga.walkList(walkToHospital_1, function(r){
				if(!r){
					cb(false);
					return;
				}
				cga.TurnTo(30, 26);
				cb(true);
			});
		});
	}
		
	//按照数组中的坐标行走
	//参数1：数组 ，例子: 
	/*const list = [
		[231, 78, null],
		[225, 70, null],
		[222, 69, null],
		[208, 67, null],
		[202, 66, null],
		[194, 58, null],
		[193, 52, null],
		[195, 50, '职业介绍所']
	];*/
	//参数2：回调函数function(result), result 为true或false
	cga.walkList = function(list, cb){
		console.log(list);
		var walkedList = new Array();
		var newList = list.slice(0);
		
		var walkCb = function(){
			if(newList.length == 0){
				//Wait 2s to avoid battle
				cb(true);
				return;
			}

			var targetX = newList[0][0];
			var targetY = newList[0][1];
			var targetMap = newList[0][2];
			
			walkedList.push(newList[0]);
			newList.shift();
			
			console.log('current: (%d, %d)', cga.GetMapXY().x, cga.GetMapXY().y);
			console.log('target: (%d, %d)', targetX, targetY);
			
			cga.AsyncWalkTo(targetX, targetY, function(r, reason){
				if(!r){
					if(reason == 2){
						//Battle, wait a second then try again
						var tm = setInterval(function(){
							if(!cga.isInBattle()){
								clearInterval(tm);
								var map = cga.GetMapName();
								var pos = cga.GetMapXY();

								const isInRange = function(x, y, startX, startY, endX, endY){
									var flags = 0;
									if(endX == startX && startX == x)
										flags |= 1;
									else if(endX > startX && x >= startX && x <= endX)
										flags |= 1;
									else if(endX < startX && x >= endX && x <= startX)
										flags |= 1;
									if(endY == startY && startY == y)
										flags |= 2;
									else if(endY > startY && y >= startY && y <= endY)
										flags |= 2;
									else if(endY < startY && y >= endY && y <= startY)
										flags |= 2;
									
									return (flags == 3) ? true : false;
								}
								for(var i = walkedList.length - 1; i > 0; --i){
									if((map == walkedList[i][2] || walkedList[i][2] == null) &&
										isInRange(pos.x, pos.y,
										walkedList[i-1][0], walkedList[i-1][1],
										walkedList[i][0], walkedList[i][1])) {

										newList.unshift(walkedList[i]);
										walkedList.splice(i, 1);
										
										walkCb();
										return;
									}
								}
								
								console.log('rollback to last');
								newList.unshift(walkedList.pop());
								walkCb();
								return;
							}
						}, 1000);
						return;
					}
					cb(r);
					return;
				}
				if(targetMap == null){
					walkCb();
					return;
				}
				//console.log('desire map = %s', targetMap);
				cga.AsyncWaitMovement({map: [targetMap], delay: 500, timeout: 5000}, function(r2){
					//console.log(r2);
					var ttt = setInterval(()=>{
						if(cga.isInBattle())
							return;
						clearInterval(ttt);
						if(r2 == true && cga.GetMapName() != targetMap){
							console.log('rollback to last 2');
							//newList.unshift(walkedList.pop());
							walkCb();
						} else {
							console.log('continue 2');
							walkCb();
						}
					}, 500);
				});
			});
		};
		walkCb();
	}
		
	//查找玩家技能，返回技能index，找不到返回-1
	//参数1：技能名
	//参数2：完全匹配
	cga.findPlayerSkill = function(name){
		var match = arguments[1] ? arguments[1] : true;
		var skills = cga.GetSkillsInfo();
		for(var i in skills){
			if(match && skills[i].name == name){
				return skills[i];
			}
			if(!match && skills[i].name.indexOf(name) != -1){
				return skills[i];
			}
		}
		return null;
	}
	
	//查找NPC，返回npc对象，找不到返回false
	//参数1：npc名字
	//参数2：模型id
	cga.findNPC = function(name){
		var model_id = arguments[1] ? arguments[1] : null;
		var units = cga.GetMapUnits();
		for(var i in units){
			if(model_id != null && units[i].model_id != model_id)
				continue;
			if(units[i].type != 2 || units[i].model_id == 0)
				continue;
			if(units[i].name == name)
				return units[i];
		}
		return false;
	}
	
	//取背包中的物品数量
	//参数1：物品名, 或#物品id
	//参数2：是否包括装备栏
	cga.getItemCount = function(name){
		var includeEquipment = arguments[1] ? arguments[1] : false;
		var items = cga.getInventoryItems();
		var count = 0;
		if(name.charAt(0) == '#'){
			var itemid = parseInt(name.substring(1));
			for(var i in items){
				if(!includeEquipment && items[i].pos < 8)
					continue;
				if(items[i].itemid == itemid)
					count += items[i].count > 0 ? items[i].count : 1;
			}
		} else {
			for(var i in items){
				if(!includeEquipment && items[i].pos < 8)
					continue;
				if(items[i].name == name)
					count += items[i].count > 0 ? items[i].count : 1;
			}
		}
		return count;
	}
	
	//任务
	cga.task = {};
	
	//任务对象构造函数
	cga.task.Task = function(name, stages, requirements){
		
		this.stages = stages;
		this.name = name;
		
		this.requirements = requirements
		
		this.isDone = function(index){
			for(var i = this.requirements.length - 1; i >= index; --i){
				if(typeof this.requirements[i] == 'function' && this.requirements[i]())
					return true;
			}
			return false;
		}
		
		this.doNext = function(index, cb){
			if(index >= this.stages.length){
				console.log('任务：'+this.name+' 已完成！');
				cb(true);
			} else {
				this.doStage(index, cb);
			}
		}
	
		this.doStage = function(index, cb){
			if(this.isDone(index)){
				console.log('第'+(index+1)+'/'+stages.length+'阶段已完成，跳过。');
				this.doNext(index+1, cb);
				return;
			}
			console.log('开始执行第'+(index+1)+'阶段：');
			console.log(this.stages[index].intro);
			var objThis = this;
			objThis.stages[index].workFunc(function(r){
				if(!r){
					cb(r);
					return;
				}
				console.log('第'+(index+1)+'阶段执行完成。');
				objThis.doNext(index + 1, cb);
			});
		}

		this.doTask = function(cb){
			console.log('任务：'+this.name+' 开始执行，共'+this.stages.length+'阶段。');
			this.doStage(0, cb);
		}
		
		return this;
	}
	
	//等待NPC出现
	cga.task.waitForNPC = function(name, cb2){
		if(cga.findNPC(name) == false){
			var timer = setInterval(function(){
				if(cga.findNPC(name) != false){
					clearInterval(timer);
					cb2();
					return;
				}
				cga.SayWords('1', 0, 0, 0);
			}, 10000);
		} else {
			cb2();
		}
	}
	
	cga.task.joinJobBattleCommon = function(jobname, cb) {
		return cga.task.Task('就职' + jobname, [
		{
			intro: '1.到法兰城的东医院[224.87]内找护士买“止痛药”',
			workFunc: function(cb2){
				cga.travel.falan.toEastHospital(function(r){
					if(!r){
						cb(false);
						return;
					}
					var npc = cga.findNPC('药剂师波洛姆');
					if(npc == false){
						cb(false);
						return;
					}
					cga.AsyncWalkTo(npc.xpos-1, npc.ypos, function(r){
						cga.TurnTo(npc.xpos, npc.ypos);
						cga.AsyncWaitNPCDialog(function(dlg){
							cga.ClickNPCDialog(0, 0);
							cga.AsyncWaitNPCDialog(function(dlg2){
								cga.BuyNPCStore([{index:1, count:1}]);
								cga.AsyncWaitNPCDialog(function(dlg3){
									if(dlg3.message.indexOf('请保重') >= 0){
										cb(true);
										return;
									}
								});
							});
						});
					});
				});
			}
		},
		{
			intro: '2.接著再到公会[73.60]，把止痛药交给安布伦后他会给你一张“通行证” ',
			workFunc: function(cb2){
				cga.travel.falan.toStone('W1', function(r){
					if(!r){
						cb(false);
						return;
					}
					const walkTo = [
						[67, 77, null],
						[73, 61, null],
						[73, 60, '职业工会'],
						[7, 18, null],
						[7, 7, null],
						[8, 6, null]
					];
					cga.walkList(walkTo, function(r){
						if(!r){
							cb2(false);
							return;
						}
						cga.TurnTo(10, 6);
						cga.AsyncWaitNPCDialog(function(dlg){
							console.log(dlg);
							cga.ClickNPCDialog(4, 0);
							cga.AsyncWaitNPCDialog(function(dlg2){
								cb(true);
							});
						});
					});
				});
			}
		},
		{
			intro: '3、出西门进国营第24坑道（351.146），在一楼左方找哈鲁迪亚说话就可以进入试练洞窟。直闯6F大厅，和波洛米亚（23.15）交谈后就可以拿到推荐信。',
			workFunc: function(cb2){
				cga.travel.falan.toStone('W1', function(r){
					if(!r){
						cb(false);
						return;
					}
					const walkTo = [
						[59, 85, null],
						[54, 87, null],
						[23, 87, null],
						[22, 87, '芙蕾雅'],
						[368, 168, null],
						[352, 146, null],
						[351, 145, '国营第24坑道 地下1楼'],
						[9, 15, null],
					];
					cga.walkList(walkTo, function(r){
						if(!r){
							cb2(false);
							return;
						}
						cga.TurnTo(9, 13);
						cga.AsyncWaitNPCDialog(function(dlg){
							cga.ClickNPCDialog(1, 0);
							cga.AsyncWaitMovement({x: 7, y: 15}, function(r){
								const walkTo2 = [
									[5, 11, null],
									[6, 7, null],
									[8, 5, null],
									[9, 5, '试炼之洞窟 第1层'],
									[11, 24, null],
									[10, 18, null],
									[13, 17, null],
									[15, 14, null],
									[23, 17, null],
									[28, 17, null],
									[28, 15, null],
									[30, 15, null],
									[30, 14, null],
									[35, 14, null],
									[37, 18, null],
									[35, 22, null],
									[33, 24, null],
									[33, 30, null],
									[33, 31, '试炼之洞窟 第2层'],
									[42, 28, null],
									[41, 24, null],
									[32, 26, null],
									[30, 28, null],
									[26, 29, null],
									[23, 33, null],
									[21, 33, null],
									[21, 41, null],
									[22, 42, '试炼之洞窟 第3层'],
									[18, 36, null],
									[19, 28, null],
									[16, 27, null],
									[11, 24, null],
									[10, 22, null],
									[11, 17, null],
									[14, 14, null],
									[21, 14, null],
									[33, 13, null],
									[38, 20, null],
									[42, 26, null],
									[42, 33, null],
									[42, 34, '试炼之洞窟 第4层'],
									[32, 41, null],
									[22, 41, null],
									[17, 39, null],
									[10, 37, null],
									[8, 34, null],
									[7, 23, null],
									[7, 20, null],
									[13, 14, null],
									[25, 14, null],
									[27, 12, '试炼之洞窟 第5层'],
									[33, 13, null],
									[34, 15, null],
									[34, 19, null],
									[35, 19, null],
									[35, 23, null],
									[27, 30, null],
									[23, 30, null],
									[14, 21, null],
									[27, 30, null],
									[13, 21, null],
									[7, 21, null],
									[4, 24, null],
									[5, 23, null],
									[11, 39, null],
									[15, 42, null],
									[20, 44, null],
									[30, 43, null],
									[36, 42, null],
									[39, 37, null],
									[39, 38, '试炼之洞窟 大厅'],
									[32, 41, null],
									[26, 38, null],
									[23, 25, null],
									[26, 38, null],
									[23, 20, null],
								];
								cga.walkList(walkTo2, function(r){
									if(!r){
										cb2(false);
										return;
									}
									var job = cga.GetPlayerInfo().job;
									if(job == '游民'){
										cga.AsyncWalkTo(23, 17, function(r){
											cga.turnDir(6);
											cga.AsyncWaitNPCDialog(function(dlg2){
												console.log(dlg2);
												cb(true);
											});
										});
									} else {
										cga.walkList([[20, 17, null], [20, 13, null], [23, 12, null]], function(r){
											cga.SayWords(jobname, 0, 0, 0);
											cga.AsyncWaitNPCDialog(function(dlg2){
												console.log(dlg2);
												cb(true);
											});
										});
									}
								});
							});
						});
					});
				});
			}	
		}
		],
		[//任务阶段是否完成
			function(){//止痛药
				return (cga.getItemCount('#18233') > 0) ? true : false;
			},
			function(){//试炼洞穴通行证
				return (cga.getItemCount('#18100') > 0) ? true : false;
			}
		]
		);
	}

	cga.gather = {};
	
	cga.gather.stats = function(itemname, itemgroupnum){
		this.begintime = moment();
		this.prevcount = cga.getItemCount(itemname);
		this.itemname = itemname;
		this.itemgroupnum = itemgroupnum;
		this.printStats = function(){
			var count = cga.getItemCount(this.itemname) - this.prevcount;
			
			console.log('一次采集完成，耗时' + moment.duration(moment() - this.begintime, 'ms').locale('zh-cn').humanize());
			console.log('获得 '+ itemname +' x '+count+'，共 ' + parseInt(count / this.itemgroupnum) + ' 组。');
			
			this.begintime = moment();
		}
		return this;
	}

	cga.craft = {}
	
	cga.craft.buyFabricLv1 = (id, count, cb)=>{
		cga.travel.falan.toFabricStore((r)=>{
			cga.TurnTo(8, 6);
			cga.AsyncWaitNPCDialog(function(dlg){
				cga.ClickNPCDialog(0, 0);
				cga.AsyncWaitNPCDialog(function(dlg2){
					cga.BuyNPCStore([{index:id, count:count}]);
					cga.AsyncWaitNPCDialog(function(dlg3){
						cb(true);
					});
				});
			});
		});
	}
	
	//搜索第一个可鉴定的物品
	cga.findAssessableItem = function(){
		var frompos = arguments[0] ? arguments[0] : 8;
		var skill = cga.findPlayerSkill('鉴定');
		var mp = cga.GetPlayerInfo().mp;
		var items = cga.getInventoryItems();
		for(var i in items){
			if(items[i].pos >= frompos && 
				false == items[i].assessed && 
				skill.lv >= items[i].level &&
				mp >= items[i].level * 10){
					
				return items[i];
			}
		}
		return null;
	}
	
	//鉴定背包中所有的物品
	cga.assessAllItems = function(cb){
		if(!cga.findPlayerSkill('鉴定')){
			cb(false, '你没有鉴定技能');
			return;
		}
		var frompos = arguments[1] ? arguments[1] : 8;
		var item = cga.findAssessableItem(frompos);
		if(item)
		{
			console.log(item);
			if(!cga.assessItem(item.pos)){
				assessAllItems(cb, item.pos + 1);
			}
			else
			{
				cga.AsyncWaitWorkingResult(function(r){
					setTimeout(function(){//delay a bit
						cga.assessAllItems(cb);
					}, 100);
				}, 30000);
			}
		} else {
			cb(true, '鉴定结束，所有物品都已鉴定完成或蓝量不足。');
			return;
		}
	}
	
	cga.findItem = (itemname) =>{
		
		var items = cga.getInventoryItems();
		
		if(itemname.charAt(0) == '#'){
			for(var i in items){
				if(items[i].itemid == itemname.substring(1)){
					return items[i].pos;
				}
			}			
			return -1;
		}
		for(var i in items){
			if(items[i].name == itemname){
				return items[i].pos;
			}
		}
		return -1;
	}
	
	cga.findBankEmptySlot = () =>{
		
		var banks = cga.GetBankItemsInfo();

		var arr = [];

		for(var i = 0; i < banks.length; ++i){
			arr[banks[i].pos-100] = true;
		}
		
		for(var i = 0; i < 80; ++i){
			if(arr[i] !== true)
				return 100+i;
		}
		
		return -1;		
	}

	cga.saveToBankOnce = (itemname, cb)=>{
		var itempos = cga.findItem(itemname);
		if(itempos == -1){
			cb(false, new Error('包里没有该物品, 无法存放到银行'));
			return;
		}
		
		var emptyslot = cga.findBankEmptySlot();
		if(emptyslot == -1){
			cb(false, new Error('银行没有空位, 无法存放到银行'));
			return;
		}
		
		cga.MoveItem(itempos, emptyslot, -1);
		var tt = setInterval(()=>{
			if(cga.GetItemInfo(emptyslot))
			{
				console.log(cga.GetItemInfo(emptyslot));
				clearInterval(tt);
				cb(true);
				return;
			}
		}, 500);
	}
	
	cga.saveToBankAll = (itemname, cb)=>{
		var repeat = ()=>{
			cga.saveToBankOnce(itemname, (r, err)=>{
				if(!r){
					cb(r, err);
					return;
				}
				if(cga.findItem(itemname) == -1){
					cb(true);
					return;
				}				
				repeat();
			});
		}
		
		repeat();		
	}
	
	cga.freqMoveDirTable = [ 4, 5, 6, 7, 0, 1, 2, 3 ];
	cga.freqMove = function(dir, cb){
		if(!cga.IsInGame()){
			cb(new Error('not in game!'));
			return;
		}
		cga.freqMoveDir = dir;
		var pos = cga.GetMapXY();
		var tt = setInterval(()=>{
			var curpos = cga.GetMapXY();
			if(cga.freqMoveDir == 0 || cga.freqMoveDir == 4){
				if(pos.x > curpos.x)
					cga.ForceMove(cga.freqMoveDir, false);
				else
					cga.ForceMove(cga.freqMoveDirTable[cga.freqMoveDir], false);
			}
			else if(cga.freqMoveDir == 2 || cga.freqMoveDir == 6){
				if(pos.y > curpos.y)
					cga.ForceMove(cga.freqMoveDir, false);
				else
					cga.ForceMove(cga.freqMoveDirTable[cga.freqMoveDir], false);
			}
			cb(tt);
		}, 100);
	}
	
	return cga;
}