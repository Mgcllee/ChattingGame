# Multi-User chatting program
---

1. 유저가 접속하는 방을 따로 만들어 관리하기
2. 채팅 중 비속어 등을 처리할 로직 만들기 ('*' 기호로 변환 등)
3. 방 생성은 조건(100명 충족 or 생성된 시간)이 충족되면 채팅 시작

<br/>

## 서버 접속 가능 클라이언트 수

최대 접속 가능 수: 50'000 클라이언트  

<br/>

> netsh int ipv4 set dynamicport tcp start=1024 num=51000

<br/>

