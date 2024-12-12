#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

// Function to load the ratings matrix from a CSV file
vector<vector<int>> loadRatings(const string& filename) {
    vector<vector<int>> ratings;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        vector<int> row;
        stringstream ss(line);
        string value;

        while (getline(ss, value, ',')) {
            row.push_back(stoi(value));
        }
        ratings.push_back(row);
    }

    return ratings;
}

// Function to calculate similarity between two users using cosine similarity
double calculateSimilarity(const vector<int>& user1, const vector<int>& user2) {
    int dotProduct = 0, sum1 = 0, sum2 = 0;

    for (size_t i = 0; i < user1.size(); ++i) {
        dotProduct += user1[i] * user2[i];
        sum1 += user1[i] * user1[i];
        sum2 += user2[i] * user2[i];
    }

    double denominator = sqrt(sum1) * sqrt(sum2);
    return (denominator == 0) ? 0 : dotProduct / denominator;
}

// Function to predict ratings for a specific user
vector<pair<int, double>> predictRatings(const vector<vector<int>>& ratings, int userId) {
    vector<double> predictedRatings(ratings[0].size(), 0.0);
    vector<double> similaritySums(ratings[0].size(), 0.0);

    for (size_t otherUser = 0; otherUser < ratings.size(); ++otherUser) {
        if (otherUser == userId) continue;

        double similarity = calculateSimilarity(ratings[userId], ratings[otherUser]);

        for (size_t movie = 0; movie < ratings[0].size(); ++movie) {
            if (ratings[userId][movie] == 0 && ratings[otherUser][movie] > 0) {
                predictedRatings[movie] += similarity * ratings[otherUser][movie];
                similaritySums[movie] += similarity;
            }
        }
    }

    vector<pair<int, double>> recommendations;
    for (size_t movie = 0; movie < predictedRatings.size(); ++movie) {
        if (ratings[userId][movie] == 0 && similaritySums[movie] > 0) {
            recommendations.emplace_back(movie, predictedRatings[movie] / similaritySums[movie]);
        }
    }

    return recommendations;
}

// Function to recommend top N movies
vector<pair<int, double>> recommendMovies(const vector<pair<int, double>>& predictions, int N) {
    vector<pair<int, double>> topN = predictions;
    sort(topN.begin(), topN.end(), [](const pair<int, double>& a, const pair<int, double>& b) {
        return b.second < a.second; // Sort in descending order of predicted rating
    });

    if (topN.size() > N) topN.resize(N);
    return topN;
}

// Main function
int main() {
    string filename = "ratings.csv";
    vector<vector<int>> ratings = loadRatings(filename);

    int userId, N;
    cout << "Enter User ID (0-based index): ";
    cin >> userId;
    cout << "Enter the number of recommendations (N): ";
    cin >> N;

    vector<pair<int, double>> predictions = predictRatings(ratings, userId);
    vector<pair<int, double>> topN = recommendMovies(predictions, N);

    cout << "\nTop " << N << " Recommended Movies for User " << userId << ":\n";
    for (const auto& rec : topN) {
        cout << "Movie " << rec.first + 1 << " with predicted rating: " << rec.second << endl;
    }

    return 0;
}
