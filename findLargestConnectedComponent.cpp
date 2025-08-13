#include <iostream>
#include <cstdint> 
#include <vector>
#include <stdexcept> 

/**
 * @class Image
 * @brief Represents a 2D image with integer pixel values.
 *
 * This class stores image data in a single contiguous block of memory
 * for efficiency, using a std::vector. It provides an At() method for
 * safe access to pixel data at specified row and column coordinates.
 */
class Image {
public:
    /**
     * @brief Constructs an Image with specified dimensions.
     * @param rows The number of rows in the image.
     * @param cols The number of columns in the image.
     */
    Image(int rows, int cols) : rows_(rows), cols_(cols), data_(rows * cols, 0) {}

    /**
     * @brief Constructs an Image with specified dimensions and initial data.
     * @param rows The number of rows in the image.
     * @param cols The number of columns in the image.
     * @param initial_data A vector containing the initial pixel values.
     */
    Image(int rows, int cols, const std::vector<int>& initial_data)
        : rows_(rows), cols_(cols), data_(initial_data) {
        // Ensure the provided data matches the dimensions
        if (data_.size() != static_cast<size_t>(rows_ * cols_)) {
            throw std::invalid_argument("Initial data size does not match image dimensions.");
        }
    }

    /**
     * @brief Accesses a pixel value at a specific location.
     * @param row The row of the pixel.
     * @param col The column of the pixel.
     * @return A reference to the pixel value, allowing it to be read or modified.
     */
    int& At(int row, int col) {
        // Perform bounds checking for safe access
        if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
            throw std::out_of_range("Image::At() - Coordinates are out of bounds.");
        }
        return data_[row * cols_ + col];
    }

    /**
     * @brief Const version of At() for read-only access on const Image objects.
     * @param row The row of the pixel.
     * @param col The column of the pixel.
     * @return A const reference to the pixel value.
     */
    const int& At(int row, int col) const {
        if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
            throw std::out_of_range("Image::At() const - Coordinates are out of bounds.");
        }
        return data_[row * cols_ + col];
    }

private:
    int rows_;
    int cols_;
    std::vector<int> data_; // Using a single vector for contiguous memory
};


// Forward declaration of the helper function
int CheckNextPixel(int rows, int cols, Image& img, int curRow, int curCol, int currentPixel, int currentCount);

/**
 * @brief Finds the largest connected area of a single pixel value in an image.
 * This function modifies the image by setting visited pixels to -1.
 */
int FindLargestConnected(int rows, int cols, Image& img) {
    int countForPixel[256] = {0}; // Assumes pixel values are 0-255
    
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int currentPixel = img.At(row, col);
            if (currentPixel == -1) continue;

            // Start a new search for a connected component
            int currentCount = 1;
            img.At(row, col) = -1; // Mark current pixel as visited

            // Start the recursive search from the current pixel
            currentCount = CheckNextPixel(rows, cols, img, row, col, currentPixel, currentCount);

            if (currentCount > countForPixel[currentPixel]) {
                countForPixel[currentPixel] = currentCount;
            }
        }
    }

    int maxCount = 0;
    for (int i = 0; i < 256; i++) {
        if (countForPixel[i] > maxCount) {
            maxCount = countForPixel[i];
        }
    }
    return maxCount;
}

/**
 * @brief Recursively checks neighboring pixels to find the size of a connected component.
 */
int CheckNextPixel(int rows, int cols, Image& img, int curRow, int curCol, int currentPixel, int currentCount) {
    int count = currentCount;
    
    // Define the 4-way neighbors (top, bottom, left, right)
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; ++i) {
        int nextRow = curRow + dr[i];
        int nextCol = curCol + dc[i];

        // Check bounds
        if (nextRow >= 0 && nextRow < rows && nextCol >= 0 && nextCol < cols) {
            if (img.At(nextRow, nextCol) == currentPixel) {
                img.At(nextRow, nextCol) = -1; // Mark as visited
                count++;
                // Recurse from the new pixel
                count = CheckNextPixel(rows, cols, img, nextRow, nextCol, currentPixel, count);
            }
        }
    }
    return count;
}


// --- Main function to demonstrate usage ---
int main() {
    int rows = 5;
    int cols = 6;

    // Sample image data
    std::vector<int> data = {
        1, 1, 2, 2, 3, 3,
        1, 1, 1, 2, 3, 3,
        4, 4, 1, 2, 2, 3,
        4, 4, 4, 5, 5, 5,
        4, 4, 4, 4, 5, 5
    };

    Image myImage(rows, cols, data);

    std::cout << "Original Image:" << std::endl;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            std::cout << myImage.At(r, c) << " ";
        }
        std::cout << std::endl;
    }
    
    // Create a copy for the function to modify
    Image imageToProcess = myImage; 
    int largestArea = FindLargestConnected(rows, cols, imageToProcess);

    std::cout << "\nLargest connected area has a size of: " << largestArea << std::endl;

    return 0;
}
