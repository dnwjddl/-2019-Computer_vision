#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
#define SIZE 3
void BrightnessControl(BYTE * In, BYTE * Out, int Val, int W, int H)
{
	int Size = W * H;
	for (int i = 0; i < Size; i++)
	{
		if (In[i] + Val > 255) Out[i] = 255;
		else if(In[i] + Val < 0) Out[i] = 0;
		else Out[i] = In[i] + Val;
	}
}
void ContrastControl(BYTE * In, BYTE * Out, double Val, int W, int H)
{
	int Size = W * H;
	for (int i = 0; i < Size; i++)
	{
		In[i] * Val > 255.0 ? Out[i] = 255 : Out[i] = (BYTE)(In[i] * Val);
	}
}
void ObtainHisto(BYTE * Img, int * Histo, int W, int H)
{
	int Size = W * H;
	for (int i = 0; i < Size; i++)
		Histo[Img[i]]++;
	FILE * fp3 = fopen("Histo.txt", "wt");
	for (int i = 0; i < 256; i++)
		fprintf(fp3, "%d\t%d\n", i, Histo[i]);
	fclose(fp3);
}
void Binarization(BYTE * In, BYTE * Out, int Th, int W, int H)
{
	int Size = W * H;
	for (int i = 0; i < Size; i++)
	{
		if(In[i] > Th) Out[i] = 255;
		else Out[i] = 0;
	}
}
void HistoStretching(BYTE * Image, BYTE * Output, int W, int H, int * Histo, double p)
{
	int High, Low;
	int ImgSize = W*H;
	int Limit = ImgSize * p;
	int cnt = 0;
	for (int i = 0; i < 256; i++) {
		cnt += Histo[i];
		if (cnt > Limit) {
			Low = i;
			break;
		}
	}
	cnt = 0;
	for (int i = 255; i >= 0; i--) {
		cnt += Histo[i];
		if (cnt > Limit) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++)
	{
		if (Image[i] < Low) Output[i] = 0;
		else if (Image[i] > High) Output[i] = 255;
		else 	Output[i] = (BYTE)(((Image[i] - Low) / (double)(High - Low)) * 255);
	}
}
void ObtainAccHisto(int * Histo, int * AHisto)
{
	AHisto[0] = Histo[0];
	for (int i = 1; i < 256; i++)
		AHisto[i] = AHisto[i - 1] + Histo[i];
}
void HistoEqualization(BYTE * Image, BYTE * Output, int W, int H, int *AHisto,
	int Gmax)
{
	int ImgSize = W*H;
	int Nt = ImgSize;
	double NR = Gmax / (double)Nt;
	int NSum[256];
	for (int i = 0; i < 256; i++)
		NSum[i] = (int)(NR * AHisto[i]);
	for (int i = 0; i < ImgSize; i++)
		Output[i] = NSum[Image[i]];
}
void LowPassFilter(BYTE * Image, BYTE * Output, double* m, 
	const int S, int W, int H)
{
	//const int Size = S;
	int Margin = SIZE / 2;
	double Mask[SIZE][SIZE];
	for (int i = 0; i < SIZE*SIZE; i++) 
		Mask[i / S][i%S] = m[i];
	double temp = 0.0;
	for (int i = Margin; i < H - Margin; i++) { // 마스크 중앙의 세로방향 이동
		for (int j = Margin; j < W - Margin; j++) { // 마스크 중앙의 가로방향 이동
			for (int m = -Margin; m <= Margin; m++) { // 마스크 중앙 기준 세로방향 주변화소 접근
				for (int n = -Margin; n <= Margin; n++) { // 마스크 중앙 기준 가로방향 주변화소 접근
					temp += (Image[(i + m)*W + (j + n)] * Mask[m + Margin][n + Margin]);
				}
			}
			Output[i*W + j] = (BYTE)temp;
			temp = 0.0;
		}
	}
}
void HighPassFilter(BYTE * Image, BYTE * Output, int* m,
	const int S, int W, int H)
{
	int Margin = SIZE / 2;
	double Mask[SIZE][SIZE];
	for (int i = 0; i < SIZE*SIZE; i++)
		Mask[i / SIZE][i%SIZE] = m[i];
	int temp = 0;
	for (int i = Margin; i < H - Margin; i++) { // 마스크 중앙의 세로방향 이동
		for (int j = Margin; j < W - Margin; j++) { // 마스크 중앙의 가로방향 이동
			for (int m = -Margin; m <= Margin; m++) { // 마스크 중앙 기준 세로방향 주변화소 접근
				for (int n = -Margin; n <= Margin; n++) { // 마스크 중앙 기준 가로방향 주변화소 접근
					temp += (Image[(i + m)*W + (j + n)] * Mask[m + Margin][n + Margin]);
				}
			}
			Output[i*W + j] = temp;
			temp = 0;
		}
	}
}

void HighPassFilter2(BYTE * Image, BYTE * Output, int* m,
	const int S, int W, int H) // 마스크 합이 1인 경우 사용할 함수
{
	int * Temp = (int *)malloc(W*H*sizeof(int));
	int Margin = SIZE / 2;
	double Mask[SIZE][SIZE];
	for (int i = 0; i < SIZE*SIZE; i++)
		Mask[i / SIZE][i%SIZE] = m[i];
	int Max = -99999, Min = 99999;
	int temp = 0;
	for (int i = Margin; i < H - Margin; i++) { // 마스크 중앙의 세로방향 이동
		for (int j = Margin; j < W - Margin; j++) { // 마스크 중앙의 가로방향 이동
			for (int m = -Margin; m <= Margin; m++) { // 마스크 중앙 기준 세로방향 주변화소 접근
				for (int n = -Margin; n <= Margin; n++) { // 마스크 중앙 기준 가로방향 주변화소 접근
					temp += (Image[(i + m)*W + (j + n)] * Mask[m + Margin][n + Margin]);
				}
			}
			if (temp > Max) Max = temp;
			if (temp < Min) Min = temp;
			Temp[i*W + j] = temp;
			temp = 0;
		}
	}
	for (int i = 0; i < W*H; i++)
		Output[i] = (Temp[i] - Min) / (double)(Max - Min) * 255;

	free(Temp);
}
void swap(BYTE * a, BYTE * b) {
	BYTE temp;
	temp = *a;
	*a = *b;
	*b = temp;
}
void Sorting(BYTE * Arr) {
	for (int i = 0; i < 8; i++) { // Pivot index
		for (int j = i + 1; j < 9; j++) { // 비교대상 index
			if (Arr[i] > Arr[j]) swap(&Arr[i], &Arr[j]);
		}
	}
}
BYTE Median(BYTE * arr) {
	Sorting(arr);
	return arr[4];
}
int push(short *stackx, short *stacky, int arr_size, short vx, short vy, int *top)
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short *stackx, short *stacky, short *vx, short *vy, int *top)
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}
// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width)
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// 스택으로 사용할 메모리 할당
	short* stackx = new short[height*width];
	short* stacky = new short[height*width];
	short* coloring = new short[height*width];

	int arr_size = height * width;

	// 라벨링된 픽셀을 저장하기 위해 메모리 할당

	for (k = 0; k<height*width; k++) coloring[k] = 0;  // 메모리 초기화

	for (i = 0; i<height; i++)
	{
		index = i*width;
		for (j = 0; j<width; j++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
			if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
			r = i; c = j; top = 0; area = 1;
			curColor++;

			while (1)
			{
			GRASSFIRE:
				for (m = r - 1; m <= r + 1; m++)
				{
					index = m*width;
					for (n = c - 1; n <= c + 1; n++)
					{
						//관심 픽셀이 영상경계를 벗어나면 처리 안함
						if (m<0 || m >= height || n<0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // 현재 라벨로 마크
							if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
							r = m; c = n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if (pop(stackx, stacky, &r, &c, &top) == -1) break;
			}
			if (curColor<1000) BlobArea[curColor] = area;
		}
	}

	float grayGap = 255.0f / (float)curColor;

	// 가장 면적이 넓은 영역을 찾아내기 위함 
	for(i=1; i<=curColor; i++)
	{
		if(BlobArea[i]>=BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage 배열 클리어~
	for (k = 0; k < width*height; k++) CutImage[k] = 255;

	// coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
	for (k = 0; k < width*height; k++)
	{
		if(coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}
// 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함
void Erosion(BYTE * Img, BYTE * Out, int W, int H)
{
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Img[i*W + j] == 255) { // 전경이라면
				if (!(Img[(i-1)*W + j] == 255 && Img[(i+1)*W + j] == 255 &&
					Img[i*W + (j-1)] == 255 && Img[i*W + (j+1)] == 255))
				{
					Out[i*W + j] = 0;
				}
				else
				{
					Out[i*W + j] = 255;
				}
			}
			else { // 배경이라면
				Out[i*W + j] = 0;
			}
		}
	}
}

void Dilation(BYTE * Img, BYTE * Out, int W, int H)
{
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Img[i*W + j] == 0) { // 배경이라면
				if (Img[(i - 1)*W + j] == 255 || Img[(i + 1)*W + j] == 255 ||
					Img[i*W + (j - 1)] == 255 || Img[i*W + (j + 1)] == 255)
				{
					Out[i*W + j] = 255;
				}
				else
				{
					Out[i*W + j] = 0;
				}
			}
			else { // 전경이라면
				Out[i*W + j] = 255;
			}
		}
	}
}
void main()
{
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes)
	FILE *fp;
	fp = fopen("face.bmp", "rb");
	if (fp == NULL) return;
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE * Image;
	BYTE * Output;
	if (hInfo.biBitCount == 8) {
		fread(hRGB, sizeof(RGBQUAD), 256, fp);
		Image = (BYTE *)malloc(ImgSize);
		Output = (BYTE *)malloc(ImgSize);
		fread(Image, sizeof(BYTE), ImgSize, fp);
	}
	else { // 트루컬러인경우
		Image = (BYTE *)malloc(ImgSize*3);
		Output = (BYTE *)malloc(ImgSize*3);
		fread(Image, sizeof(BYTE), ImgSize*3, fp);
	}
	fclose(fp);
	int W = hInfo.biWidth, H = hInfo.biHeight;

	/* 영상처리 */
	
	BYTE*YCbCr = (BYTE*)malloc(ImgSize * 3);
	for (int i = 0; i < ImgSize * 3; i = i + 3) { //YCbCr로 바꿔주기
		YCbCr[i] = (0.2999 *Image[i + 2]) + (0.587*Image[i + 1]) + (0.114*Image[i]);
		YCbCr[i + 1] = -0.16874*Image[i + 2] - 0.3313*Image[i + 1] + (0.5*Image[i]) + 128;
		YCbCr[i + 2] = 0.5*Image[i + 2] - 0.4187*Image[i + 1] - 0.0813*Image[i] + 128;
	}

	for (int i = 0; i < ImgSize * 3; i += 3) {
		if ((YCbCr[i + 1] > 95) && (YCbCr[i + 1] < 125) && (YCbCr[i + 2]>155) && (YCbCr[i + 2] < 185)) {//살색 도출+이진화
			Image[i] = 255;//B
			Image[i + 1] = 255;//G
			Image[i + 2] = 255;//R
		}
		else {
			Image[i] = 0;
			Image[i + 1] = 0;
			Image[i + 2] = 0;
		}
	}
	int y1, y2, x1, x2;
	for (int j = 0; j < H; j++) {
		for (int i = 0; i < W; i++) {
			if (Image[i*W + j] == 255) {
				y1 = j;
			}
			break;
	}
	}
	for (int j = H; j >0; j--) {
		for (int i = W; i >0; i--) {
			if (Image[i*W + j] == 255) {
				y2 = j;
			}
			break;
		}
	}
	for (int i=0; i < W; i++) {
		for (int j = 0; j <H; j++) {
			if (Image[i*W + j] == 255) {
				x1 = j;
			}
			break;
		}
	}
	for (int i = W; i >0; i--) {
		for (int j = H; j >0; j--) {
			if (Image[i*W + j] == 255) {
				x2 = j;
			}
			break;
		}
	}
	for (int i = x1; i < x2; i++) {
		Image[i*W + y1] = 255;
		Image[i*W + y2] = 255;
	}
	for (int i = y1; i < y2; i++) {
		Image[x1*W+i] = 255;
		Image[x2*W +i] = 255;
	}


	for (int i = 0; i < ImgSize * 3; i++) {
		Output[i] = Image[i];
	}



	/* 영상처리 */
	fp = fopen("output.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	if (hInfo.biBitCount == 8) {
		fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(Output, sizeof(BYTE), ImgSize, fp);
	}
	else {
		fwrite(Output, sizeof(BYTE), ImgSize*3, fp);
	}
	fclose(fp);
	free(Image);
	free(Output);
}