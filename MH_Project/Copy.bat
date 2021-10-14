// 명령어			옵션				원본이 있는 경로			복사본을 저장할 경로

xcopy			/y				.\Engine\Headers\*.h			.\Reference\Headers\
xcopy			/y				.\Engine\Headers\*.inl			.\Reference\Headers\



xcopy			/y				.\Engine\System\Codes\*.h			.\Reference\Headers\
xcopy			/y				.\Engine\System\Bin\System.dll		.\Client\Bin\
xcopy			/y				.\Engine\System\Bin\System.dll		.\MFCTool\Bin\
xcopy			/y				.\Engine\System\Bin\System.lib		.\Reference\Library\

