var cga = require('./cgaapi')(function(){
	
	var healme = function(){
		
		var skill = cga.findPlayerSkill('治疗');
		
		var requiremp = 25 + cga.GetSkillInfo(skill).lv * 5;
		
		if (cga.GetPlayerInfo().mp < requiremp){
			cga.TurnTo(35, 88);
			setTimeout(function(){
				healme();
			}, 5000);
			return;
		}
		
		cga.StartWork(skill, 0);
		cga.AsyncWaitPlayerMenu(function(players){
			//console.log(players);
			cga.PlayerMenuSelect(0);
			cga.AsyncWaitUnitMenu(function(units){
				//console.log(units);
				cga.UnitMenuSelect(0);
				cga.AsyncWaitWorkingResult(function(r){
					//console.log(r);
					if(cga.GetPlayerInfo().health != 0)
						healme();
				});
			});
		});
	}
	
	healme();
});