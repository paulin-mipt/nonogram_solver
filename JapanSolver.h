#include <vector>

class JapanSolver {
	std::vector< std::vector<int> > pict_numbers, tb_res;
	std::vector< std::vector< std::vector<int> > > Lines;
	std::vector< std::vector<bool> > need_refresh;
	std::vector<int> cells, bl_len;
	std::vector<bool> can_one, can_zero;
	int blocksNum, XSz, YSz, size;
	bool sol_found;
	int ErrorLevel;
public:
	JapanSolver(std::vector< std::vector<int> > numbers, int _blocksNum) {
		blocksNum = _blocksNum;
		XSz = numbers.size() - blocksNum;
		YSz = numbers[0].size() - blocksNum;
		if (XSz > YSz) 
			size = XSz;
		else
			size = YSz;

		can_one.resize(size + 2);
		can_zero.resize(size + 2);
		cells.resize(size + 2);

		Lines.resize(2);
		need_refresh.resize(2);
		Lines[1].resize(XSz);
		Lines[0].resize(YSz);
		need_refresh[1].resize(XSz);
		need_refresh[0].resize(YSz);
		pict_numbers.resize(size + 1);
		for (int i = 0; i < size + 1; i++) {
			pict_numbers[i].resize(size + 1);
		}
		tb_res.resize(blocksNum + 1);
		bl_len.resize(blocksNum + 1);

		for (int i = 0; i < tb_res.size(); i++) {
			tb_res[i].resize(size + 1);
		}
		for (int i = 0; i < Lines[0].size(); i++) {
			for (int j = 0; j < blocksNum; j++)
				if (numbers[blocksNum - j - 1][i + blocksNum]) {
					Lines[0][i].push_back(numbers[blocksNum - j - 1][i + blocksNum]);
					need_refresh[0][i] = true;
				}
		}

		for (int i = 0; i < Lines[1].size(); i++) {
			for (int j = 0; j < blocksNum; j++)
				if (numbers[i + blocksNum][blocksNum - j - 1]) {
					Lines[1][i].push_back(numbers[i + blocksNum][blocksNum - j - 1]);
					need_refresh[1][i] = true;
				}
		}
	
		for (int i = 0; i < XSz; i++) {
			for (int j = 0; j < YSz; j++) {
				switch (numbers[i + blocksNum][j + blocksNum]) {
				case 1:
					pict_numbers[i][j] = 1;
					break;
				case 2:
					pict_numbers[i][j] = 0;
					break;
				default:
					pict_numbers[i][j] = 2;
					break;
				}
			}
		}
		sol_found = false;
	}

	bool TryBlock(int theblock, int thestart, int N, int L) {
		int startnext;
		bool res;
		if (theblock > 0 && tb_res[theblock][(thestart + 1)] != -1) {
			return (tb_res[theblock][(thestart + 1)] == 1);
		}
		for (int i = (thestart); i < (thestart) + bl_len[theblock]; i++) {
			if (cells[i + 1] == 0) {
				tb_res[theblock][(thestart + 1)] = 0;
				return false;
			}
			if (theblock < N - 1) {
				res = false;
				for (startnext = thestart + bl_len[theblock] + 1; startnext <= L - bl_len[theblock + 1] + 1; startnext++) {
					if (cells[startnext] == 1) break;
					if (TryBlock(theblock + 1, startnext, N, L)) {
						res = true;
						for (int i = (thestart); i < (thestart) + bl_len[theblock]; i++) {
							can_one[i + 1] = true;
						}
						for (int i = (thestart) + bl_len[theblock]; i < startnext; i++) {
							can_zero[i + 1] = true;
						}
					}
				}
				return res;
			}
			else {
				for (int i = (thestart) + bl_len[theblock]; i <= L; i++) {
					if (cells[i + 1] == 1)
						return false;
					for (int i = (thestart); i < (thestart) + bl_len[theblock]; i++) {
						can_one[i + 1] = true;
					}
					for (int i = (thestart) + bl_len[theblock]; i <= L; i++) {
						can_zero[i + 1] = true;
					}
				}
				return true;
			}
		}
		return false;
	}

	void AnalyzeLine(int kind, int number) {
		bl_len = std::vector<int>(blocksNum + 1);
		int N, L;
		need_refresh[kind][number] = false;
		if (kind == 1)
			L = XSz;
		else
			L = YSz;

		cells[0] = 1;
		cells[1] = 0;
		if (kind == 1) {
			for (int i = 0; i < L; i++) {
				cells[i + 1] = pict_numbers[number][i];
			}
		}
		else {
			for (int i = 0; i < L; i++)  {
				cells[i + 1] = pict_numbers[i][number];
			}
		}
		for (int i = -1; i < L; i++)  {
			can_one[i + 1] = false;
			can_zero[i + 1] = false;
		}
		N = Lines[kind][number].size();
		bl_len[0] = 1;
		for (int i = 0; i < N; i++) {
			bl_len[i + 1] = Lines[kind][number][i];
		}

		for (int i = 0; i < N; i++) {
			for (int j = 0; j <= L; j++) {
				tb_res[i][j] = -1;
			}
		}
		if (TryBlock(0, -1, N, L)) {
			for (int i = 0; i < L; i++) {
				if (cells[i + 1] == 2 && (can_one[i + 1] ^ can_zero[i + 1]))  {
					need_refresh[1 - kind][i] = true;
					if (can_one[i + 1])
						cells[i + 1] = 1;
					else
						cells[i + 1] = 0;
					if (kind == 1)
						pict_numbers[number][i] = cells[i + 1];
					else
						pict_numbers[i][number] = cells[i + 1];
				}
			}
		} else ErrorLevel = 1;
	}

	void IterateLineLook() {
		bool sl = true;
		while (sl) {
			sl = false;
			for (int i = 0; i < YSz && !sl; i++) {
				if (need_refresh[0][i]) {
					AnalyzeLine(0, i);
					sl = true;
				}
			}
			for (int i = 0; i < XSz; i++) {
				if (need_refresh[1][i]) {
					AnalyzeLine(1, i);
					sl = true;
				}
			}
		}
	}


	void Try(int y, int x)  {
		ErrorLevel = 0;
		IterateLineLook();
		if (ErrorLevel)
			return;
		int j = y, i = x;
		std::vector<int> p(size * size + 1);
		while (j < YSz && pict_numbers[j][i] != 2) {
			if (i == XSz - 1) {
				i = 0;
				j++;
			}
			else {
				i++;
			}
		}
		if (j >= YSz)
			return;
		else {
			for (int j_ = 0; j_ < YSz; j_++) {
				for (int i_ = 0; i_ < XSz; i_++) {
					p[XSz * (j_) + i_] = pict_numbers[j_][i_];
				}
			}
			pict_numbers[j][i] = 0;
			need_refresh[0][j] = true;
			need_refresh[1][i] = true;
			Try(j, i);
			for (int j_ = 0; j_ < YSz; j_++) {
				for (int i_ = 0; i_ < XSz; i_++) {
					pict_numbers[j_][i_] = p[XSz*(j_) + i_];
					pict_numbers[j][i] = 1;
					need_refresh[0][j] = true; need_refresh[1][i] = true;
					Try(j, i);
				}
			}
		}
	}

	bool solve(std::vector< std::vector<int> > *numbers) {
		Try(0, 0);
		if (sol_found) {
			for (int i = 0; i < XSz; i++) {
				for (int j = 0; j < YSz; j++) {
					switch (pict_numbers[i][j]) {
					case 1:
						(*numbers)[i + blocksNum][j + blocksNum] = 1;
						break;
					case 0:
						(*numbers)[i + blocksNum][j + blocksNum] = 2;
						break;
					default:
						(*numbers)[i + blocksNum][j + blocksNum] = 0;
					}
				}
			}
			return true;
		}
		else {
			return false;
		}
	}
};
