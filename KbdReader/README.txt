[개발 환경]

Visusl Studio 2015
WDK 7600.16385.1


[테스트 방법]

1. 소스 코드 빌드

2. 테스트 대상 시스템의 플랫폼과 일치하는 다음 파일들을 테스트 시스템의 동일 폴더에 위치시킴
 - KbdReader.exe 
 - KbdRdr.sys
 - KnCommLibUser.dll
 - install.bat
 - uninstall.bat

3. install.bat 관리자 권한으로 실행

4. KbdReader.exe 실행

5. Start 버튼 클릭

6. Stop 버튼 클릭

7. KbdReader.exe 종료

8. uninstall.bat 관리자 권한으로 실행