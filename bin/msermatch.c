#include "ccv.h"
#include <sys/time.h>

unsigned int get_current_time()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

unsigned char colors[6][3] = {{0,0,255},{0,255,0},{255,0,0},{255,255,0},{255,0,255},{0,255,255}};

int main(int argc, char** argv)
{
	assert(argc == 3);
	ccv_enable_default_cache();
	ccv_dense_matrix_t* image = 0;
	ccv_read(argv[1], &image, CCV_IO_COLOR | CCV_IO_ANY_FILE);
	ccv_mser_param_t params = {
		.min_area = 60,
		.max_area = (int)(image->rows * image->cols * 0.3 + 0.5),
		.min_diversity = 0.2,
		.area_threshold = 1.01,
		.min_margin = 0.003,
		.max_evolution = 200,
		.edge_blur_sigma = sqrt(5.0),
	};
	ccv_dense_matrix_t* mser = 0;
	ccv_array_t* mser_keypoint = ccv_mser(image, 0, &mser, 0, params);
	ccv_dense_matrix_t* graph = ccv_dense_matrix_new(image->rows, image->cols, CCV_8U | CCV_C3, 0, 0);
	ccv_matrix_free(image);
	ccv_array_free(mser_keypoint);
	int i, j;
	for (i = 0; i < graph->rows; i++)
		for (j = 0; j < graph->cols; j++)
		{
			if (mser->data.i32[i * mser->cols + j] == 0)
			{
				graph->data.u8[i * graph->step + j * 3] =
				graph->data.u8[i * graph->step + j * 3 + 1] =
				graph->data.u8[i * graph->step + j * 3 + 2] = 255;
			} else {
				graph->data.u8[i * graph->step + j * 3] = colors[mser->data.i32[i * mser->cols + j] % 6][0];
				graph->data.u8[i * graph->step + j * 3 + 1] = colors[mser->data.i32[i * mser->cols + j] % 6][1];
				graph->data.u8[i * graph->step + j * 3 + 2] = colors[mser->data.i32[i * mser->cols + j] % 6][2];
			}
		}
	ccv_write(graph, argv[2], 0, CCV_IO_PNG_FILE, 0);
	ccv_matrix_free(graph);
	ccv_disable_cache();
	return 0;
}
