{
  "targets": [
    {
      "target_name": "node_cga",
      "sources": [ "main.cc", "connect.cc", "item.cc", "info.cc", "map.cc", "npc.cc", "work.cc", "chat.cc", "battle.cc"  ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
		"../",
		"../rest_rpc/",		
		"../rest_rpc/spdlog/include/",
		"../rest_rpc/iguana/",
		"../rest_rpc/iguana/third_party/msgpack/include/",
		"../boost/"
      ],
	  
	  "conditions": [
                [
                    "OS=='win'",
                    {
                        "link_settings": {
                            "libraries": [
								"-lcgalib"
                            ]
                        },
                        "configurations": {
                            "Debug": {
                                "msvs_settings": {
                                    "VCCLCompilerTool": {
										"RuntimeLibrary" : "3",
                                        "ExceptionHandling": "1",
										"RuntimeTypeInfo": "true",
                                        "AdditionalOptions": [
                                            "/MP"
                                        ]
                                    },
                                    "VCLibrarianTool": {
                                        "AdditionalOptions": [
                                        ]
                                    },
                                    "VCLinkerTool": {
                                        "LinkTimeCodeGeneration": 1,
                                        "LinkIncremental": 1,
                                        "AdditionalLibraryDirectories": [
                                            "../../x64/Debug/",
											"../../Debug/",
                                            "../../boost/stage/lib"
                                        ]
                                    }
                                }
                            },
                            "Release": {
                                "msvs_settings": {
                                    "VCCLCompilerTool": {
										"RuntimeLibrary" : "2",
										'ExceptionHandling': "1",
                                        "AdditionalOptions": [
                                            "/MP"
                                        ]
                                    },
                                    "VCLibrarianTool": {
                                        "AdditionalOptions": [
                                        ]
                                    },
                                    "VCLinkerTool": {
                                        "LinkTimeCodeGeneration": 1,
                                        "AdditionalLibraryDirectories": [
                                            "../../x64/Release/",
                                            "../../Release/",
											"../../boost/stage/lib"
                                        ]
                                    }
                                }
                            }
                        }
                    }
                ]
            ]
	  
    }
  ]
}
