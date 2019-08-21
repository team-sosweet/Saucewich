# Saucewich

UE4로 개발중인 캐주얼 멀티플레이어 TPS 게임

## 빌드

Pull을 받고 나면 반드시 rebuild를 해야합니다.

### For Non-Programmers

1. `Binaries` 폴더가 있으면 삭제
2. `Saucewich.uproject` 열기
3. `Missing Saucewich Modules. Would you like to rebuild them now?` 와 같은 창이 뜨면 `예(Y)` 누르기

### For Programmers (Visual Stuido)

1. `Saucewich.uproject` 우클릭
2. `Generate Visual Studio project files` 클릭
3. `Saucewich.sln` 열기
4. Saucewich 빌드

## Pull Request

각 팀원들은 자신에게 주어진 branch에서 작업하게 됩니다. 하나의 작업이 끝나면 master branch로 pull request를 날려야 합니다.

Pull request는 자주 날릴수록 좋습니다. 보통 commit은 컴파일/빌드에 문제가 없는 수준의 단위로, pull request는 하나의 기능이 완성되어 문제 없이 정상적으로 동작하는 단위로 날리는게 좋습니다.

게시된 pull request는 수시로 검토되며 문제가 없다고 판단되면 수락되어 merge됩니다. 만약 conflict가 발생할 경우 스스로 해결하시길 바랍니다. conflict가 발생하는 pull request는 거절될 수 있습니다.
