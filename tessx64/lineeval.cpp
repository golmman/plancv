#include "lineeval.h"
#include "bresenham.h"


// evaluates lines
void lines_evaluate(PlanData& data, const vector<LineSeg>& lines) {

	// prepare buffer
	int scale = data.info.scale;
	int scaled_rows = data.height / scale + 2;
	int scaled_cols = data.width  / scale + 2;

	int* buffer = new int[scaled_rows * scaled_cols];

	// create plan chunks
	for(size_t i = 0; i < lines.size(); ++i) {
		const LineSeg& l = lines[i];
		data.chunks.push_back(PlanChunk(l.sx, l.sy, l.ex, l.ey, 0));
	}

	// move lines a bit to eliminate errors at scale edges
	for (int y = 0; y < scale; ++y) {
		for (int x = 0; x < scale; ++x) {

			// reset votes
			memset(buffer, 0, 4 * scaled_rows * scaled_cols);

			// vote all lines
			for(size_t i = 0; i < lines.size(); ++i) {
				const LineSeg& l = lines[i];
				bhm_vote(buffer, scaled_cols, 
					(l.sx + x) / scale, 
					(l.sy + y) / scale, 
					(l.ex + x) / scale, 
					(l.ey + y) / scale);
			}

			// score lines
			for(size_t i = 0; i < lines.size(); ++i) {
				const LineSeg& l = lines[i];

				double score = bhm_score(buffer, scaled_cols, 
					(l.sx + x) / scale, 
					(l.sy + y) / scale, 
					(l.ex + x) / scale, 
					(l.ey + y) / scale);

				data.chunks[i].linescore += score;
				//data.chunks.push_back(PlanChunk(l.sx, l.sy, l.ex, l.ey, score));
			}

		}
	}

	// normalize line score
	for(size_t i = 0; i < lines.size(); ++i) {
		data.chunks[i].linescore /= (scale * scale);
	}




	delete[] buffer;
}