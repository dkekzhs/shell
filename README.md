# shell
오류.. 우선순위 해결
1. ex) !한글입력 -> if(atoi(한글입력) == 0) continue While 처음으로 갔을 때 historyPath =력으로 바뀐다..
    history 입력할때 문자열 입력한후 버퍼제거 해야됨
3. ps 두번치면 꺼짐 파일 닫는거 문제있는거같음


ps 구현 방법 uid or ttyname 
tty로 함수 만들면 self로 tty)self 가져오고 dir(/proc)에서 숫자로 되어있는 것들 중에서 tty랑 tty)self 비교 같다면 stat 열어서 pid와 name 가져와서 구현 // 끝

