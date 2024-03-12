#include <stdio.h>

#include <stdlib.h>

#include <time.h>

#include "raylib.h"

#include "raymath.h"

#define RAYLIB_STATIC

#define SATIR 15
#define SUTUN 15

const int ekranGenisligi = 600;
const int ekranYuksekligi = 600;

const int kareGenisligi = ekranGenisligi / SUTUN  ;
const int kareYuksekligi = ekranYuksekligi / SATIR;

const char* kaybetme = " KAYBETTINIZ !";             // Oyun kayip edildiginde ekrana yazilacak metin.
const char* kazanma = "TEBRIKLER KAZANDINIZ";          // Oyun kazanildiginde ekrana yazilacak metin.
const char* yenidenBaslat = "Yeniden Oynamak Icin 'Y' Tusuna Basiniz";       // Oyunun yeniden baslatilmasi için   ekrana yazilacak metin.

float oyunBaslamaZamani;  // Oyun sonunda ekrana yazilacak olan oynanis süresinin baslangici için degisken tanimlanmasi.
float oyunBitisZamani;   // Oyun sonunda ekrana yazilacak olan oynanis süresinin bitisi için degisken tanimlanmasi.
int ortayaCikanKare;
int mayinOlan;
Texture2D mayin;
Texture2D bayrak;


typedef struct Kare         // Kare adinda tanimlanan struct , karelerle ilgili  6 ozelligi tutuyor.
{
	int i;
	int j;
	bool mayinli;
	bool acilmis;
	bool bayrakli;
	int yanindakimayinsayisi;
} Kare;

Kare kareler[SUTUN][SATIR];      // Kare yapi türüden kareler adinda bir yapi dizisi olusturuluyor.

typedef enum OyunDurumu      // Oyunun durumunu belirlemek icin enum yapisinda bir OyunDurumu turu tanimlaiyor.
{
	OYNANIYOR,
	KAYBEDILDI,
	KAZANILDI
    
} OyunDurumu;

OyunDurumu durum;    // OyunDurumu turunde bir durum degiskeni olusturuluyor.

void OyunBaslangici(void);   // Oyun Baslangicinda calistirilan ; oyun ekranini ve karelelerin ilk durumini ayarlayan fonksiyon.
void KareIslemleri(void);  // Karelerdeki mayin,bayrak ve acilma islemlerini saglayan fonksiyon.
void KareAcma(int, int);    // Tiklanan karenin acilmasinin saglayan fonksiyon.
void KareCiz(Kare);    // Oyun Zeminini cizilmesini saglayan fonksiyon.
int MayinliHucreSayisi(int, int);  // Mayinli hucre sayisini hesaplayan fonksiyon.
void BosKareTemizleme(int, int);   // Acilan ve mayin olmayan kareleri temizliyen fonksiyon.
bool TiklananYerKontrol(int, int);  // Kullanicinin tikladigi yerin oyun ekrani icerisinde olup olmadigini kontrol eden fonskiyon.
void BayrakKontrol(int, int);  // Karelere bayrak konulup konulmadigini kontrol eden fonksiyon.


int main()
{
    
	InitWindow(ekranGenisligi, ekranYuksekligi, "MAYIN TARLASI");  // Pencere uzerindeki yazının ayarlanmasi.
    Image logo = LoadImage("eklentiler/logo1.png");   // Oyunda kullanilan bayrak ve mayin resimlerinin yuklenmesi.
    SetWindowIcon(logo);
    UnloadImage(logo);

    mayin = LoadTexture("eklentiler/resim1.png");
	bayrak = LoadTexture("eklentiler/flag.png");
    
    srand(time(0));

	OyunBaslangici();
	
	while(!WindowShouldClose())  // Oyun penceersi kapatilana kadar mouse ile yapilan islemlerin kontrolu.
	{
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			Vector2 mPos = GetMousePosition();
			int indexI = mPos.x / kareGenisligi;
			int indexJ = mPos.y / kareYuksekligi;

			if (durum == OYNANIYOR && TiklananYerKontrol(indexI, indexJ))
			{
				KareAcma(indexI, indexJ);
			}
		}
		else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			Vector2 mPos = GetMousePosition();
			int indexI = mPos.x / kareGenisligi;
			int indexJ = mPos.y / kareYuksekligi;

			if (durum == OYNANIYOR && TiklananYerKontrol(indexI, indexJ))
			{
				BayrakKontrol(indexI, indexJ);
			}
		}

		if (IsKeyPressed(KEY_Y))
		{
			OyunBaslangici();
		}

		BeginDrawing();  // Oyun Durumuna gore gerekli cizimlerin yapilmasi.

			ClearBackground(BLACK);
			
			for (int i = 0; i < SUTUN ; i++)
			{
				for (int j = 0; j < SATIR; j++)
				{
					KareCiz(kareler[i][j]);
				}
			}

			if (durum == KAYBEDILDI)
			{
				DrawRectangle(0, 0, ekranGenisligi,ekranYuksekligi, Fade(WHITE, 0.8f));
				DrawText(kaybetme, ekranGenisligi / 2 - MeasureText(kaybetme, 40) / 2, ekranYuksekligi / 2 - 10, 40, RED);
				DrawText(yenidenBaslat, ekranGenisligi / 2 - MeasureText(yenidenBaslat, 20) / 2, ekranYuksekligi * 0.75f - 10, 20, BLACK);

				int dakika = (int)(oyunBitisZamani - oyunBaslamaZamani) / 60;
				int saniye = (int)(oyunBitisZamani - oyunBaslamaZamani) % 60;
				DrawText(TextFormat("Oynan Sure: %d dakika, %d saniye", dakika, saniye), 20, ekranYuksekligi - 40, 20, DARKGRAY);
                
                
                
			}

			if (durum == KAZANILDI)
			{
				DrawRectangle(0, 0, ekranGenisligi,ekranYuksekligi, Fade(WHITE, 0.8f));
				DrawText(kazanma, ekranGenisligi / 2 - MeasureText(kazanma, 40) / 2, ekranYuksekligi / 2 - 10, 40, GREEN);
				DrawText(yenidenBaslat, ekranGenisligi / 2 - MeasureText(yenidenBaslat, 20) / 2, ekranYuksekligi * 0.75f - 10, 20, BLACK);

				int dakika = (int)(oyunBitisZamani - oyunBaslamaZamani) / 60;
				int saniye = (int)(oyunBitisZamani - oyunBaslamaZamani) % 60;
				DrawText(TextFormat("Oynan Sure: %d dakika, %d saniye", dakika, saniye), 20, ekranYuksekligi - 40, 20, DARKGRAY);
			}
            
            

		EndDrawing();
	}
    
    
    
	
	CloseWindow();
	
	return 0;
}


// FONKSIYONLAR "ACIKLAMA" DOSYASINDA DETAYLI BIR SEKILDE ACIKLANMISTIR.

void OyunBaslangici(void)
{
	KareIslemleri();
	durum = OYNANIYOR;
	ortayaCikanKare = 0;
	oyunBaslamaZamani = GetTime();
}



void KareIslemleri(void)
{
	for (int i = 0; i < SUTUN ; i++)
	{
		for (int j = 0; j < SATIR; j++)
		{
			kareler[i][j] = (Kare)
			{
				.i = i,
				.j = j,
				.mayinli = false,
				.acilmis = false,
				.bayrakli = false,
				.yanindakimayinsayisi = -1
			};
		}
	}

	mayinOlan = (int)(SATIR * SUTUN * 0.1f);
	int yerlestirilecekMayinSayisi = mayinOlan;
	while (yerlestirilecekMayinSayisi > 0)
	{
		int i = rand() % SUTUN ;
		int j = rand() % SATIR;

		if (!kareler[i][j].mayinli)
		{
			kareler[i][j].mayinli = true;
			yerlestirilecekMayinSayisi--;
		}
	}

	for (int i = 0; i < SUTUN ; i++)
	{
		for (int j = 0; j < SATIR; j++)
		{
			if (!kareler[i][j].mayinli)
			{
				kareler[i][j].yanindakimayinsayisi = MayinliHucreSayisi(i, j);
			}
		}
	}
}


void KareCiz(Kare kare)
{
	if (kare.acilmis)
	{
		if (kare.mayinli)
		{
			DrawRectangle(kare.i * kareGenisligi, kare.j * kareYuksekligi, kareGenisligi, kareYuksekligi, RED);
            
            Rectangle source = {0, 0, mayin.width, mayin.height};
            Rectangle dest = {kare.i * kareGenisligi, kare.j * kareYuksekligi, kareGenisligi, kareYuksekligi};
            Vector2 origin = {0, 0};
            DrawTexturePro(mayin, source, dest, origin, 0.0f, Fade(WHITE, 0.5f));
		}
		else
		{
			DrawRectangle(kare.i * kareGenisligi, kare.j * kareYuksekligi, kareGenisligi, kareYuksekligi, RAYWHITE);

			if (kare.yanindakimayinsayisi > 0)
			{
				DrawText(TextFormat("%d", kare.yanindakimayinsayisi), kare.i * kareGenisligi + 12, kare.j * kareYuksekligi + 4, kareYuksekligi - 8, DARKGRAY);
			}
		}
	}
	else if (kare.bayrakli)
	{
		
		Rectangle source = {0, 0, bayrak.width, bayrak.height};
		Rectangle dest = {kare.i * kareGenisligi, kare.j * kareYuksekligi, kareGenisligi, kareYuksekligi};
		Vector2 origin = {0, 0};

		DrawTexturePro(bayrak, source, dest, origin, 0.0f, Fade(WHITE, 0.5f));
	}

	DrawRectangleLines(kare.i * kareGenisligi, kare.j * kareYuksekligi, kareGenisligi, kareYuksekligi, RED);
}


void KareAcma(int i, int j)
{
	if (kareler[i][j].bayrakli || kareler[i][j].acilmis)
	{
		return;
	}

	kareler[i][j].acilmis = true;

	if (kareler[i][j].mayinli)
	{
		durum = KAYBEDILDI;
		oyunBitisZamani = GetTime();
	}
	else
	{
		if (kareler[i][j].yanindakimayinsayisi == 0)
		{
			BosKareTemizleme(i, j);
		}

		ortayaCikanKare++;

		if (ortayaCikanKare >= SATIR * SUTUN  - mayinOlan)
		{
			durum = KAZANILDI;
			oyunBitisZamani = GetTime();
		}
	}
}



int MayinliHucreSayisi(int i, int j)
{
	int sayac = 0;
	for (int SatirKontrol = -1; SatirKontrol <= 1; SatirKontrol++)
	{
		for (int SutunKontrol = -1; SutunKontrol <= 1; SutunKontrol++)
		{
			if (SatirKontrol == 0 && SutunKontrol == 0)
			{
				continue;
			}

			if (!TiklananYerKontrol(i + SatirKontrol, j + SutunKontrol))
			{
				continue;
			}

			if (kareler[i + SatirKontrol][j + SutunKontrol].mayinli)
			{
				sayac++;
			}
		}
	}

	return sayac;
}



void BosKareTemizleme(int i, int j)
{
	for (int SatirKontrol = -1; SatirKontrol <= 1; SatirKontrol++)
	{
		for (int SutunKontrol = -1; SutunKontrol <= 1; SutunKontrol++)
		{
			if (SatirKontrol == 0 && SutunKontrol == 0)
			{
				continue;
			}

			if (!TiklananYerKontrol(i + SatirKontrol, j + SutunKontrol))
			{
				continue;
			}

			KareAcma(i + SatirKontrol, j + SutunKontrol);
		}
	}
}


void BayrakKontrol(int i, int j)
{
	if (kareler[i][j].acilmis)
	{
		return;
	}

	kareler[i][j].bayrakli = !kareler[i][j].bayrakli;
}



bool TiklananYerKontrol(int i, int j)
{
	return i >= 0 && i < SUTUN  && j >= 0 && j < SATIR;
}








