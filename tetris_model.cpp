// tetris_model.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <math.h>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <functional>


using namespace std;

double f(double x, double angle) {
	return (-1.0) * x / tan(angle) + 1.0;
}

double g(double x, double angle) {
	return  (x - 1.0) / tan(angle) + 1.0;
}

class part{
public:
	double x;
	double y;
	int r;
	int orinum;

	static bool comparepart(part lpart, part rpart) { return (lpart.r < rpart.r); }
	
	part();
};

part::part() {
	x = -1;
	y = -1;
	r = -1;
	orinum = -1;
}


int main()
{
	// parameter1
	int npatch_1d, npart, num_skip_IC, num_step, skip, movie_switch, neighborhood;
	double rangle, langle, py;
	

	// input and output
	cout << "input npatch_1d : ";
	cin >> npatch_1d;
	cout << "R_wall_tilt_angle : ";
	cin >> rangle;
	cout << "L_wall_tilt_angle : ";
	cin >> langle;
	cout << "py : ";
	cin >> py;
	cout << "npart : ";
	cin >> npart;
	cout << "num_skip_IC : ";
	cin >> num_skip_IC;
	cout << "num_step : ";
	cin >> num_step;
	cout << "skip : ";
	cin >> skip;
	cout << "movie_switch : ";
	cin >> movie_switch;
	cout << "neighborhood [1:von-neuman, 2:moore] : ";
	cin >> neighborhood;

	
	// file
	FILE *fp;
	char fname[60];
	cout << "str1 : ";
	cin >> fname;
	errno_t error;
	if (error = fopen_s(&fp, fname, "w") != 0) {
		cout << fname << "cannot open!" << endl;
		return -1;
	}
	fprintf(fp, "x,y,color\n");
	
	// parameter2
	vector<vector<int>> status(npatch_1d, vector<int>(npatch_1d));
	double L, Lx_s, Ly_s, Lx, Ly, patch_halfsize;
	


	// environment
	L = 1.0 / npatch_1d;
	patch_halfsize = L / 2.0;

	for (int i = 0; i < npatch_1d; i++) {
		for (int j = 0; j < npatch_1d; j++) {
			Lx_s = i * L;
			Lx = Lx_s + L / 2.0;
			Ly_s = j * L;
			Ly = Ly_s + L / 2.0;

			
			if (f(Lx, rangle) <= Ly and g(Lx, rangle) <= Ly) {
				status[i][j] = 1;
			}

			else {
				status[i][j] = 0;
			}
			
			
		}
	}


	// inserting particles
	vector<part> particle(npart);
	
	srand(time(NULL));

	for (int i = 0; i < npart; i++) {
		particle[i].orinum = i;
		while (1) {
			int x_r, y_r;
			x_r = rand() % npatch_1d;
			y_r = rand() % npatch_1d;
			if (status[x_r][y_r] == 1) {
				particle[i].x = x_r;
				particle[i].y = y_r;
				if (i == 0) {
					status[x_r][y_r] = 3;
				}
				else {
					status[x_r][y_r] = 2;
				}
				break;
			}
		}
	}

	for (int i = 0; i < npatch_1d; i++) {
		for (int j = 0; j < npatch_1d; j++) {
			if (status[i][j] == 3) {
				fprintf(fp, "%f, %f,red\n", float(i) / float(npatch_1d), float(j) / float(npatch_1d));
			}
			else if (status[i][j] == 2) {
				fprintf(fp, "%f, %f,blue\n", float(i) / float(npatch_1d), float(j) / float(npatch_1d));
			}
			else if (status[i][j] == 1) {
				fprintf(fp, "%f, %f,lightgreen\n", float(i) / float(npatch_1d), float(j) / float(npatch_1d));
			}
			else {
				fprintf(fp, "%f, %f,dimgray\n", float(i) / float(npatch_1d), float(j) / float(npatch_1d));
			}
		}
	}


	// MC_step
	vector<int> randnum(npart);

	for (int num = 0; num < num_step; num++) {
		int counter;
		for (int i = 0; i < npart; i++) {
			particle[i].r = rand() % npart;
		}

		sort(particle.begin(), particle.end(), part::comparepart);
		
		for (int cnt = 0; cnt < npart; cnt++) {
			int np = particle[cnt].orinum;
			int xp = particle[cnt].x;
			int yp = particle[cnt].y;
			double rp = double(rand() % 1000) / 1000.0;
			
			if (rp < py) {
				// von-neuman moving method
				if (neighborhood == 1) {
					if (yp == 0) {
						while (1) {
							int new_x;
							new_x = rand() % (npatch_1d / 2) + npatch_1d / 4;
							if (status[new_x][npatch_1d - 1] == 1) {
								particle[cnt].x = new_x;
								particle[cnt].y = npatch_1d - 1;
								status[xp][yp] = 1;
								if (np == 0) {
									status[new_x][npatch_1d - 1] = 3;

								}
								else {
									status[new_x][npatch_1d - 1] = 2;
								}
								break;
							}
						}
					}
					else if (status[xp][yp - 1] == 1) {
						particle[cnt].y = yp - 1;
						status[xp][yp] = 1;
						if (np == 0) {
							status[xp][yp - 1] = 3;
						}
						else {
							status[xp][yp - 1] = 2;
						}
					}
				}

				// moore moving method
				else {
					if (rp < py / 3) {
						if (yp == 0) {
							while (1) {
								int new_x;
								new_x = rand() % (npatch_1d / 2) + npatch_1d / 4;
								if (status[new_x][npatch_1d - 1] == 1) {
									particle[cnt].x = new_x;
									particle[cnt].y = npatch_1d - 1;
									status[xp][yp] = 1;
									if (np == 0) {
										status[new_x][npatch_1d - 1] = 3;

									}
									else {
										status[new_x][npatch_1d - 1] = 2;
									}
									break;
								}
							}
						}
						else if (status[xp][yp - 1] == 1) {
							particle[cnt].y = yp - 1;
							status[xp][yp] = 1;
							if (np == 0) {
								status[xp][yp - 1] = 3;
							}
							else {
								status[xp][yp - 1] = 2;
							}
						}
					}
					else if (py / 3 <= rp and rp < 2 * py / 3) {
						if (yp == 0) {
							while (1) {
								int new_x;
								new_x = rand() % (npatch_1d / 2) + npatch_1d / 4;
								if (status[new_x][npatch_1d - 1] == 1) {
									particle[cnt].x = new_x;
									particle[cnt].y = npatch_1d - 1;
									status[xp][yp] = 1;
									if (np == 0) {
										status[new_x][npatch_1d - 1] = 3;

									}
									else {
										status[new_x][npatch_1d - 1] = 2;
									}
									break;
								}
							}
						}
						else if (xp == 0) {
							particle[cnt].x = xp;
							particle[cnt].y = yp;
						}
						else if (status[xp - 1][yp - 1] == 1) {
							particle[cnt].x = xp - 1;
							particle[cnt].y = yp - 1;
							status[xp][yp] = 1;
							if (np == 0) {
								status[xp - 1][yp - 1] = 3;
							}
							else {
								status[xp - 1][yp - 1] = 2;
							}
						}
					}
					else {
						if (yp == 0) {
							while (1) {
								int new_x;
								new_x = rand() % (npatch_1d / 2) + npatch_1d / 4;
								if (status[new_x][npatch_1d - 1] == 1) {
									particle[cnt].x = new_x;
									particle[cnt].y = npatch_1d - 1;
									status[xp][yp] = 1;
									if (np == 0) {
										status[new_x][npatch_1d - 1] = 3;

									}
									else {
										status[new_x][npatch_1d - 1] = 2;
									}
									break;
								}
							}
						}
						else if (xp == npatch_1d - 1) {
							particle[cnt].x = xp;
							particle[cnt].y = yp;
						}
						else if (status[xp + 1][yp - 1] == 1) {
							particle[cnt].x = xp + 1;
							particle[cnt].y = yp - 1;
							status[xp][yp] = 1;
							if (np == 0) {
								status[xp + 1][yp - 1] = 3;
							}
							else {
								status[xp + 1][yp - 1] = 2;
							}
						}
					}
				}

			}

			// common moving method
			else if (rp >= py and rp < 0.5 + py / 2) {
				if (xp == 0) {
					particle[cnt].x = xp;
				}
				else if (status[xp - 1][yp] == 1) {
					particle[cnt].x = xp - 1;
					status[xp][yp] = 1;
					if (np == 0) {
						status[xp - 1][yp] = 3;
					}
					else {
						status[xp - 1][yp] = 2;
					}
				}
			}
			else {
				if (xp == npatch_1d - 1)  {
					particle[cnt].x = xp;
				}
				else if (status[xp + 1][yp] == 1) {
					particle[cnt].x = xp + 1;
					status[xp][yp] = 1;
					if (np == 0) {
						status[xp + 1][yp] = 3;
					}
					else {
						status[xp + 1][yp] = 2;
					}
				}
			}
		}

		for (int i = 0; i < npatch_1d; i++) {
			for (int j = 0; j < npatch_1d; j++) {
				if (status[i][j] == 3) {
					fprintf(fp, "%f, %f,red\n", float(i) / float(npatch_1d), float(j) / float(npatch_1d));
				}
				else if (status[i][j] == 2) {
					fprintf(fp, "%f, %f,blue\n", float(i) / float(npatch_1d), float(j) / float(npatch_1d));
				}
				else if (status[i][j] == 1) {
					fprintf(fp, "%f, %f,lightgreen\n", float(i) / float(npatch_1d), float(j) / float(npatch_1d));
				}
				else {
					fprintf(fp, "%f, %f,dimgray\n", float(i) / float(npatch_1d), float(j) / float(npatch_1d));
				}
			}
		}
		
	}

	fclose(fp);

    return 0;
}

