# [2019]Computer_vision

## [2019-1]: C언어를 사용하여 동공, 얼굴 검출 직접 구현

### 피부 영역 검출
BMP 파일에서 피부 영역을 검출한 후, 피부 영역에 외접하는 바운딩 박스를 빨간색으로 표시하는 프로그램 작성

[피부 검출 코드](https://github.com/dnwjddl/-2019-Computer_vision/tree/master/%ED%94%BC%EB%B6%80%EA%B2%80%EC%B6%9C)

---
[참고문헌: HSI, RGB, YCbCr등 다양한 컬러모델에서의 피부색 range](https://arxiv.org/ftp/arxiv/papers/1708/1708.02694.pdf) <br>

---

### 동공 중심 검출
동공 중심을 지나는 cross 표시 그리기 <br>
[동공 중심 코드](https://github.com/dnwjddl/-2019-Computer_vision/tree/master/%EB%8F%99%EA%B3%B5%EC%A4%91%EC%8B%AC)<br>

---

### Sobel 가로 세로 경계 검출 합친 결과 프로그램
Sobel-X 마스크 컨볼루션 결과와 Sobel-Y 마스크 컨볼루션 결과를 하나로 합치고, 경계 영상을(적절한 임계치로) 이진화한 결과를 출력 <br>
[Sobel 마스크](https://github.com/dnwjddl/-2019-Computer_vision/tree/master/Sobel%20%EB%A7%88%EC%8A%A4%ED%81%AC)
