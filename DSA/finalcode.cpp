#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

class Movie
{
public:
    string title;
    float vote_average;
    int vote_count;
    string status;
    string release_date;
    long long revenue; // Change type to long long
    int runtime;
    bool adult;
    long long budget; // Change type to long long
    string original_language;
    string original_title;
    string overview;
    float popularity;
    string tagline;
    vector<string> genres;
    vector<string> production_companies;
    vector<string> production_countries;
    vector<string> spoken_languages;
};

// Structure to store title words and their frequency
struct WordFrequency
{
    string word;
    int frequency;
};

struct CorrelationResult
{
    double correlation_coefficient;
    double sum_x;
    double sum_y;
    double sum_xy;
    double sum_x2;
    double sum_y2;
    double denominator;
    double slope;
    double intercept;
};

// Struct to hold production company information
struct CompanyInfo
{
    string name;
    long long totalRevenue;
    vector<string> producedCountries;
};

// Function to remove double quotes from the start and end of a string
string removeQuotes(const string &str)
{
    string cleaned_str = str;

    // Remove leading and trailing quotes if present
    if (!cleaned_str.empty())
    {
        if (cleaned_str.front() == '"' && cleaned_str.back() == '"')
        {
            cleaned_str = cleaned_str.substr(1, cleaned_str.size() - 2);
        }
    }

    return cleaned_str;
}

// Function to remove leading and trailing spaces from a string
string trim(const string &str)
{
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    if (start == string::npos || end == string::npos)
    {
        return "";
    }
    return str.substr(start, end - start + 1);
}

// Function to split a string by a delimiter and return tokens as vector
vector<string> split_strings(const string &line, char delimiter, const vector<bool> &remove_quotes)
{
    vector<string> tokens;
    stringstream ss(line);
    string token;
    size_t index = 0;
    bool in_quotes = false; // Flag to track if currently inside quoted text
    while (getline(ss, token, delimiter))
    {
        // Check if removal of quotes is required for this token
        if (!remove_quotes.empty() && index < remove_quotes.size() && remove_quotes[index])
        {
            token = removeQuotes(token);
        }

        // If inside quoted text, continue reading until ending quote is found
        while (token.front() == '"' && !in_quotes)
        {
            // Check if token ends with double quote
            if (token.back() == '"')
            {
                // Remove double quotes from token
                token.erase(token.begin());
                token.pop_back();
                break;
            }
            else
            {
                // Read next token and append to current token
                string next_token;
                if (!getline(ss, next_token, delimiter))
                {
                    cerr << "Error: Ending quote not found for token: " << token << endl;
                    break;
                }
                token += delimiter + next_token;
            }
        }

        // Check if current token starts a quoted text
        if (token.front() == '"' && !in_quotes)
        {
            in_quotes = true;
        }

        // Check if current token ends the quoted text
        if (token.back() == '"' && in_quotes)
        {
            in_quotes = false;
        }

        tokens.push_back(token);
        index++;
    }
    return tokens;
}

// Function to split a string by a delimiter and return non-empty tokens as vector
vector<string> split(const string &line, char delimiter)
{
    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (getline(ss, token, delimiter))
    {
        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Function to parse the CSV file and store contents
vector<Movie> parseCSV(const string &filename, vector<bool> &remove_quotes)
{
    vector<Movie> movies;

    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return movies;
    }

    string line;
    // Skip header line
    getline(file, line);
    while (getline(file, line))
    {
        Movie movie;
        vector<string> tokens = split_strings(line, ',', remove_quotes);

        // Fill movie struct with tokens
        movie.title = tokens[1];
        movie.vote_average = stof(tokens[2]); // Convert to float
        movie.vote_count = stoi(tokens[3]);   // Convert to float
        movie.status = tokens[4];
        movie.release_date = tokens[5];
        movie.revenue = stoll(tokens[6]); // Convert to long long
        movie.runtime = stoi(tokens[7]);
        movie.adult = (tokens[8] == "False" || tokens[8] == "0");
        movie.budget = stoll(tokens[10]); // Convert to long long
        movie.original_language = tokens[13];
        movie.original_title = tokens[14];
        if (!tokens[15].empty())
            movie.overview = tokens[15];
        if (!tokens[16].empty())
        {

            movie.popularity = stof(tokens[16]); // Convert to float
        }
        if (!tokens[18].empty())
            movie.tagline = tokens[18];

        // Parse genres
        vector<string> genres = split(tokens[19], '|');
        movie.genres = move(genres);

        // Parse production companies
        vector<string> production_companies = split(tokens[20], '|');
        if (!tokens[20].empty())
            movie.production_companies = move(production_companies);

        // Parse production countries
        vector<string> production_countries = split(tokens[21], '|');
        if (!tokens[21].empty())
            movie.production_countries = move(production_countries);

        // Parse spoken languages
        vector<string> spoken_languages = split(tokens[22], '|');
        if (!tokens[22].empty())
            movie.spoken_languages = move(spoken_languages);

        movies.push_back(move(movie));
    }

    file.close();

    return movies;
}

// Function to count the frequency of all genres mentioned in the movies
void mergeString(vector<string> &arr, int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<string> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSortString(vector<string> &arr, int left, int right)
{
    if (left >= right)
    {
        return;
    }

    int mid = left + (right - left) / 2;
    mergeSortString(arr, left, mid);
    mergeSortString(arr, mid + 1, right);
    mergeString(arr, left, mid, right);
}

// Merge function for Merge Sort
void mergeInt(vector<pair<int, vector<WordFrequency>>> &arr, int low, int mid, int high)
{
    int n1 = mid - low + 1;
    int n2 = high - mid;

    // Create temporary arrays
    vector<pair<int, vector<WordFrequency>>> left(n1);
    vector<pair<int, vector<WordFrequency>>> right(n2);

    // Copy data to temporary arrays left[] and right[]
    for (int i = 0; i < n1; ++i)
        left[i] = arr[low + i];
    for (int j = 0; j < n2; ++j)
        right[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[low..high]
    int i = 0;   // Initial index of first subarray
    int j = 0;   // Initial index of second subarray
    int k = low; // Initial index of merged subarray

    while (i < n1 && j < n2)
    {
        if (left[i].first >= right[j].first)
        {
            arr[k] = left[i];
            ++i;
        }
        else
        {
            arr[k] = right[j];
            ++j;
        }
        ++k;
    }

    // Copy the remaining elements of left[], if any
    while (i < n1)
    {
        arr[k] = left[i];
        ++i;
        ++k;
    }

    // Copy the remaining elements of right[], if any
    while (j < n2)
    {
        arr[k] = right[j];
        ++j;
        ++k;
    }
}

// Merge Sort function
void mergeSortInt(vector<pair<int, vector<WordFrequency>>> &arr, int low, int high)
{
    if (low < high)
    {
        // Find the middle point
        int mid = low + (high - low) / 2;

        // Sort first and second halves
        mergeSortInt(arr, low, mid);
        mergeSortInt(arr, mid + 1, high);

        // Merge the sorted halves
        mergeInt(arr, low, mid, high);
    }
}

// Function to merge two sorted vectors of movies based on revenue
vector<Movie> mergeMovie(vector<Movie> &left, vector<Movie> &right, string column)
{
    vector<Movie> merged;
    int i = 0, j = 0;
    bool compareLeft = (column == "revenue");
    while (i < left.size() && j < right.size())
    {
        if ((compareLeft && left[i].revenue >= right[j].revenue) || (!compareLeft && left[i].popularity >= right[j].popularity))
        {
            merged.push_back(left[i]);
            i++;
        }
        else
        {
            merged.push_back(right[j]);
            j++;
        }
    }
    // Copy remaining elements from left and right vectors
    while (i < left.size())
    {
        merged.push_back(left[i]);
        i++;
    }
    while (j < right.size())
    {
        merged.push_back(right[j]);
        j++;
    }
    return merged;
}

// Function to perform merge sort on vector of movies based on revenue
vector<Movie> mergeSortMovie(vector<Movie> &movies, string column)
{
    if (movies.size() <= 1)
    {
        return movies;
    }
    int mid = movies.size() / 2;
    vector<Movie> left(movies.begin(), movies.begin() + mid);
    vector<Movie> right(movies.begin() + mid, movies.end());
    left = mergeSortMovie(left, column);
    right = mergeSortMovie(right, column);
    return mergeMovie(left, right, column);
}

// Function to merge two sorted pairs based on a numeric value
template <typename T>
void mergePairs(vector<pair<string, T>> &arr, int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<pair<string, T>> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        if (L[i].second >= R[j].second)
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Function to perform merge sort on pairs based on a numeric value
template <typename T>
void mergeSortPairs(vector<pair<string, T>> &arr, int left, int right)
{
    if (left >= right)
    {
        return;
    }

    int mid = left + (right - left) / 2;
    mergeSortPairs(arr, left, mid);
    mergeSortPairs(arr, mid + 1, right);
    mergePairs(arr, left, mid, right);
}

// Function to merge two sorted vectors of companies based on total revenue
vector<CompanyInfo> mergeCompanies(vector<CompanyInfo> &left, vector<CompanyInfo> &right)
{
    vector<CompanyInfo> merged;
    int i = 0, j = 0;
    while (i < left.size() && j < right.size())
    {
        if (left[i].totalRevenue >= right[j].totalRevenue)
        {
            merged.push_back(left[i]);
            i++;
        }
        else
        {
            merged.push_back(right[j]);
            j++;
        }
    }
    while (i < left.size())
    {
        merged.push_back(left[i]);
        i++;
    }
    while (j < right.size())
    {
        merged.push_back(right[j]);
        j++;
    }
    return merged;
}

// Function to perform merge sort on vector of companies based on total revenue
vector<CompanyInfo> mergeSortCompanies(vector<CompanyInfo> &companies)
{
    if (companies.size() <= 1)
    {
        return companies;
    }
    int mid = companies.size() / 2;
    vector<CompanyInfo> left(companies.begin(), companies.begin() + mid);
    vector<CompanyInfo> right(companies.begin() + mid, companies.end());
    left = mergeSortCompanies(left);
    right = mergeSortCompanies(right);
    return mergeCompanies(left, right);
}

// Merge function for Merge Sort - For the sorting the frequencies of words
void merge(vector<WordFrequency> &arr, int low, int mid, int high)
{
    int n1 = mid - low + 1;
    int n2 = high - mid;

    // Create temporary arrays
    vector<WordFrequency> left(n1);
    vector<WordFrequency> right(n2);

    // Copy data to temporary arrays left[] and right[]
    for (int i = 0; i < n1; ++i)
        left[i] = arr[low + i];
    for (int j = 0; j < n2; ++j)
        right[j] = arr[mid + 1 + j];

    // Merge the temporary arrays back into arr[low..high]
    int i = 0;   // Initial index of first subarray
    int j = 0;   // Initial index of second subarray
    int k = low; // Initial index of merged subarray

    while (i < n1 && j < n2)
    {
        if (left[i].frequency >= right[j].frequency)
        {
            arr[k] = left[i];
            ++i;
        }
        else
        {
            arr[k] = right[j];
            ++j;
        }
        ++k;
    }

    // Copy the remaining elements of left[], if any
    while (i < n1)
    {
        arr[k] = left[i];
        ++i;
        ++k;
    }

    // Copy the remaining elements of right[], if any
    while (j < n2)
    {
        arr[k] = right[j];
        ++j;
        ++k;
    }
}

// Merge Sort function - For the sorting the frequencies of words
void mergeSort(vector<WordFrequency> &arr, int low, int high)
{
    if (low < high)
    {
        // Find the middle point
        int mid = low + (high - low) / 2;

        // Sort first and second halves
        mergeSort(arr, low, mid);
        mergeSort(arr, mid + 1, high);

        // Merge the sorted halves
        merge(arr, low, mid, high);
    }
}

// Binary search to check if a string exists in a sorted vector of strings
bool binarySearch(const vector<string> &arr, const string &target)
{
    int left = 0;
    int right = arr.size() - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;

        // Check if the middle element is the target
        if (arr[mid] == target)
        {
            return true;
        }
        // If target is greater, ignore left half
        else if (arr[mid] < target)
        {
            left = mid + 1;
        }
        // If target is smaller, ignore right half
        else
        {
            right = mid - 1;
        }
    }
    // If not found, return false
    return false;
}

// Function to insert a string into the bucket at the appropriate position
void insertSort(vector<string> &bucket, string &val)
{
    auto it = bucket.begin();
    while (it != bucket.end() && *it < val)
    {
        ++it;
    }
    bucket.insert(it, val);
}

// Bucket sort for lowercase words
void bucketSort(vector<string> &words)
{
    vector<vector<string>> buckets(26); // Assuming only lowercase words

    // Distribute words into buckets based on their first character
    for (string &word : words)
    {
        int bucketIndex = word[0] - 'a';
        insertSort(buckets[bucketIndex], word);
    }

    // Concatenate the buckets back into the original array
    int index = 0;
    for (auto &bucket : buckets)
    {
        for (string &word : bucket)
        {
            words[index++] = word;
        }
    }
}

// Function to find the country with the highest value based on a numeric property
template <typename T>
string findCountryWithHighestProperty(const vector<Movie> &movies, T Movie::*property)
{
    vector<pair<string, T>> countryProperty;

    // Calculate total property for each country
    for (const Movie &movie : movies)
    {
        for (const string &country : movie.production_countries)
        {
            bool found = false;
            for (pair<string, T> &cp : countryProperty)
            {
                if (cp.first == country)
                {
                    cp.second += movie.*property;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                countryProperty.emplace_back(country, movie.*property);
            }
        }
    }

    // Sort countries based on the property using merge sort
    mergeSortPairs(countryProperty, 0, countryProperty.size() - 1);

    // Return the country with the highest property
    return countryProperty[0].first;
}

// Function to find the country with the most number of movies
string findMostProducingCountry(const vector<Movie> &movies)
{
    vector<pair<string, int>> countryCount;

    // Count movies for each country
    for (const Movie &movie : movies)
    {
        for (const string &country : movie.production_countries)
        {
            bool found = false;
            for (pair<string, int> &cc : countryCount)
            {
                if (cc.first == country)
                {
                    cc.second++;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                countryCount.emplace_back(country, 1);
            }
        }
    }

    // Sort countries based on movie count using merge sort
    mergeSortPairs(countryCount, 0, countryCount.size() - 1);

    // Return the country with the most number of movies
    return countryCount[0].first;
}

vector<CompanyInfo> processCompanyInfo(const vector<Movie> &movies)
{
    vector<CompanyInfo> companies;

    for (const Movie &movie : movies)
    {
        for (const string &company : movie.production_companies)
        {
            bool found = false;
            for (CompanyInfo &info : companies)
            {
                if (info.name == company)
                {
                    info.totalRevenue += movie.revenue;
                    bool countryFound = false;
                    for (const string &country : movie.production_countries)
                    {
                        countryFound = false;
                        for (const string &producedCountry : info.producedCountries)
                        {
                            if (producedCountry == country)
                            {
                                countryFound = true;
                                break;
                            }
                        }
                        if (!countryFound)
                        {
                            info.producedCountries.push_back(country);
                        }
                    }
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                CompanyInfo companyInfo;
                companyInfo.name = company;
                companyInfo.totalRevenue = movie.revenue;
                companyInfo.producedCountries = movie.production_countries;
                companies.push_back(companyInfo);
            }
        }
    }

    return companies;
}

// Overloaded function for correlation calculation with detailed output
template <typename T1, typename T2>
CorrelationResult correlation(const vector<T1> &x, const vector<T2> &y)
{
    size_t n = x.size(); // Use size_t for consistency and to avoid precision loss

    // Initialize correlation and linear regression details
    CorrelationResult result;
    result.sum_x = 0;
    result.sum_y = 0;
    result.sum_xy = 0;
    result.sum_x2 = 0;
    result.sum_y2 = 0;
    result.slope = 0;
    result.intercept = 0;

    // Check if the sizes of x and y are the same
    if (n != y.size())
    {
        cerr << "Error: Vectors x and y must have the same size for correlation calculation." << endl;
        result.correlation_coefficient = 0.0; // Return 0 for error
        return result;
    }
    // Calculate sums and products
    for (size_t i = 0; i < n; ++i)
    {
        result.sum_x += x[i];
        result.sum_y += y[i];
        result.sum_xy += x[i] * y[i];
        result.sum_x2 += x[i] * x[i];
        result.sum_y2 += y[i] * y[i];
    }

    // Calculate correlation coefficient
    result.denominator = sqrt((n * result.sum_x2 - result.sum_x * result.sum_x) * (n * result.sum_y2 - result.sum_y * result.sum_y));
    if (result.denominator == 0)
    {
        result.correlation_coefficient = 0.0; // Avoid division by zero
    }
    else
    {
        result.correlation_coefficient = (n * result.sum_xy - result.sum_x * result.sum_y) / result.denominator;
        // Calculate linear regression parameters (slope and intercept)
        result.slope = (n * result.sum_xy - result.sum_x * result.sum_y) / (n * result.sum_x2 - result.sum_x * result.sum_x);
        result.intercept = (result.sum_y - result.slope * result.sum_x) / n;
    }

    cout << "Correlation Coefficient: " << result.correlation_coefficient << endl;
    cout << "Sum of x: " << result.sum_x << endl;
    cout << "Sum of y: " << result.sum_y << endl;
    cout << "Sum of xy: " << result.sum_xy << endl;
    cout << "Sum of x^2: " << result.sum_x2 << endl;
    cout << "Sum of y^2: " << result.sum_y2 << endl;
    cout << "Denominator: " << result.denominator << endl;
    cout << "Linear Regression Equation: y = " << result.slope << "x + " << result.intercept << endl;
    cout << endl;

    return result;
}

// Function to count the frequencies of each genre
void countAllGenresFrequency(const vector<Movie> &movies)
{
    vector<string> allGenres;

    for (const auto &movie : movies)
    {
        for (const auto &genre : movie.genres)
        {
            // Split the genre string by comma and add individual genres to the list
            stringstream ss(genre);
            string individualGenre;
            while (getline(ss, individualGenre, ','))
            {
                allGenres.push_back(trim(individualGenre));
            }
        }
    }
    mergeSortString(allGenres, 0, static_cast<int>(allGenres.size()) - 1);

    // Count the frequency of each genre
    int count = 1;
    for (size_t i = 1; i < allGenres.size(); ++i)
    {
        if (allGenres[i] != allGenres[i - 1])
        {
            cout << allGenres[i - 1] << ": " << count << endl;
            count = 1;
        }
        else
        {
            count++;
        }
    }

    // Print the last genre and its count
    if (!allGenres.empty())
    {
        cout << allGenres.back() << ": " << count << endl;
    }
}

// Function to count the frequency of release years
void countReleaseYearFrequency(const vector<Movie> &movies)
{
    // Count the frequency of release years manually
    vector<pair<int, int>> yearFrequency; // Pair to store year and its frequency

    for (const auto &movie : movies)
    {
        string releaseDate = movie.release_date;
        if (releaseDate.size() >= 4)
        {
            int year = stoi(releaseDate.substr(releaseDate.size() - 4)); // Extract last four characters as year

            bool found = false;
            for (auto &freq : yearFrequency)
            {
                if (freq.first == year)
                {
                    freq.second++;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                yearFrequency.emplace_back(year, 1);
            }
        }
    }

    // Sort the yearFrequency vector in descending order
    for (size_t i = 0; i < yearFrequency.size(); ++i)
    {
        for (size_t j = i + 1; j < yearFrequency.size(); ++j)
        {
            if (yearFrequency[i].second < yearFrequency[j].second)
            {
                swap(yearFrequency[i], yearFrequency[j]);
            }
        }
    }

    // Print the year frequencies in descending order
    for (const auto &yearFreq : yearFrequency)
    {
        cout << yearFreq.first << ": " << yearFreq.second << endl;
    }
}

// Function to insert a new WordFrequency into the vector of structs
void insert(vector<WordFrequency> &wordFreq, const string &word)
{
    for (auto &wf : wordFreq)
    {
        if (wf.word == word)
        {
            wf.frequency++;
            return;
        }
    }
    wordFreq.push_back({word, 1});
}

// Function to count word frequencies in movie titles, ignoring certain words
vector<WordFrequency> countWords(const vector<Movie> &movies, vector<string> &ignoredWords)
{
    vector<WordFrequency> wordFreq;

    // Iterate through each movie
    for (const Movie &movie : movies)
    {
        stringstream ss(movie.title); // Convert title to stringstream for easy tokenization
        string word;

        // Iterate through each word in the movie title
        while (ss >> word)
        {
            // Convert word to lowercase
            string lowercaseWord;
            for (char c : word)
            {
                lowercaseWord += static_cast<char>(tolower(static_cast<unsigned char>(c)));
            }

            // If the word is not in the ignored list, update its frequency
            if (!binarySearch(ignoredWords, lowercaseWord))
            {
                // Check if the lowercase word already exists in wordFreq
                bool found = false;
                for (WordFrequency &wf : wordFreq)
                {
                    if (wf.word == lowercaseWord)
                    {
                        wf.frequency++;
                        found = true;
                        break;
                    }
                }

                // If the lowercase word doesn't exist, add it to wordFreq
                if (!found)
                {
                    wordFreq.push_back({lowercaseWord, 1});
                }
            }
        }
    }

    return wordFreq;
}

// Function to count the freqencies of words in titles according to each original language
vector<vector<WordFrequency>> countTitleWordsByOriginalLanguage(const vector<Movie> &movies, vector<string> &ignoredLanguages, vector<string> &ignoredWords)
{
    vector<vector<WordFrequency>> languageTitleWordFreq;

    bucketSort(ignoredLanguages);

    for (const Movie &movie : movies)
    {
        bool skipMovie = false;
        // Check if the current movie's original language is in the list of ignored languages
        if (binarySearch(ignoredLanguages, movie.original_language))
        {
            skipMovie = true;
        }
        if (skipMovie)
        {
            continue; // Skip processing for this movie
        }

        stringstream ss(movie.title);
        string titleWord;

        while (ss >> titleWord)
        {
            // Convert word to lowercase and remove leading/trailing punctuation marks
            string lowercaseWord;
            for (char c : titleWord)
            {
                if (isalnum(c)) // Check if the character is alphanumeric
                {
                    lowercaseWord += tolower(c); // Convert to lowercase
                }
                else if (!lowercaseWord.empty()) // Only append non-leading/trailing punctuation
                {
                    break;
                }
            }

            // Check if the lowercase word is not in the list of ignored words
            if (!binarySearch(ignoredWords, lowercaseWord) && !lowercaseWord.empty())
            {
                // Find or create language entry
                bool foundLanguage = false;
                for (vector<WordFrequency> &languageEntry : languageTitleWordFreq)
                {
                    if (languageEntry[0].word == movie.original_language)
                    {
                        foundLanguage = true;
                        // Update word frequencies for this original language
                        bool foundWord = false;
                        for (size_t i = 1; i < languageEntry.size(); i++)
                        {
                            if (languageEntry[i].word == lowercaseWord)
                            {
                                foundWord = true;
                                // Increment word frequency
                                languageEntry[i].frequency++;
                                break;
                            }
                        }
                        if (!foundWord)
                        {
                            // Add new word and frequency
                            languageEntry.push_back({lowercaseWord, 1});
                        }
                        break;
                    }
                }
                if (!foundLanguage)
                {
                    // Create new language entry
                    vector<WordFrequency> newLanguageEntry;
                    newLanguageEntry.push_back({movie.original_language, 1});
                    newLanguageEntry.push_back({lowercaseWord, 1});
                    languageTitleWordFreq.push_back(newLanguageEntry);
                }
            }
        }
    }

    return languageTitleWordFreq;
}

// Function to count word frequencies in movie titles segregated by year
vector<pair<int, vector<WordFrequency>>> countTitleWordsByYear(const vector<Movie> &movies, vector<string> &ignoredWords)
{

    vector<pair<int, vector<WordFrequency>>> yearTitleWordFreq;
    vector<int> excludedYears = {
        1911,
        1912,
        1912,
        1913,
        1914,
        2024,
        2025,
        2026};

    for (const Movie &movie : movies)
    {
        if (!movie.release_date.empty())
        {
            // Extract the substring representing the year by iterating backward from the end of the string
            string yearString;
            int lastIndex = movie.release_date.size() - 1;
            for (int i = lastIndex; i >= 0; --i)
            {
                if (movie.release_date[i] == '/')
                {
                    break; // Stop when encountering the first '/'
                }
                yearString = movie.release_date[i] + yearString; // Prepend the character to the yearString
            }

            // Convert the year string to an integer
            int releaseYear = stoi(yearString);
            // Check if the release year is not in the excluded years list
            bool isExcluded = false;
            for (int year : excludedYears)
            {
                if (year == releaseYear)
                {
                    isExcluded = true;
                    break;
                }
            }

            if (!isExcluded)
            {
                // Find or create an entry for the release year
                bool foundYear = false;
                for (auto &yearEntry : yearTitleWordFreq)
                {
                    if (yearEntry.first == releaseYear)
                    {
                        foundYear = true;
                        // Process the movie title and update word frequencies for the release year
                        for (const auto &word : split(movie.title, ' '))
                        {
                            // Convert word to lowercase and remove leading/trailing punctuation marks
                            string lowercaseWord;
                            for (char c : word)
                            {
                                if (isalnum(c)) // Check if the character is alphanumeric
                                {
                                    lowercaseWord += tolower(c); // Convert to lowercase
                                }
                                else if (!lowercaseWord.empty()) // Only append non-leading/trailing punctuation
                                {
                                    break;
                                }
                            }

                            // Check if the lowercase word is not in the list of ignored words
                            if (!binarySearch(ignoredWords, lowercaseWord) && !lowercaseWord.empty())
                            {
                                // Find the word frequency entry for the current word in the release year
                                bool foundWord = false;
                                for (auto &wf : yearEntry.second)
                                {
                                    if (wf.word == lowercaseWord)
                                    {
                                        wf.frequency++;
                                        foundWord = true;
                                        break;
                                    }
                                }
                                if (!foundWord)
                                {
                                    // If the word doesn't exist, add it with frequency 1
                                    yearEntry.second.push_back({lowercaseWord, 1});
                                }
                            }
                        }
                        break;
                    }
                }

                if (!foundYear)
                {
                    // Create a new entry for the release year
                    vector<WordFrequency> wordFreq;
                    for (const auto &word : split(movie.title, ' '))
                    {
                        // Convert word to lowercase and remove leading/trailing punctuation marks
                        string lowercaseWord;
                        for (char c : word)
                        {
                            if (isalnum(c)) // Check if the character is alphanumeric
                            {
                                lowercaseWord += tolower(c); // Convert to lowercase
                            }
                            else if (!lowercaseWord.empty()) // Only append non-leading/trailing punctuation
                            {
                                break;
                            }
                        }

                        // Check if the lowercase word is not in the list of ignored words
                        if (!binarySearch(ignoredWords, lowercaseWord) && !lowercaseWord.empty())
                        {
                            // Find the word frequency entry for the current word in the release year
                            bool foundWord = false;
                            for (auto &wf : wordFreq)
                            {
                                if (wf.word == lowercaseWord)
                                {
                                    wf.frequency++;
                                    foundWord = true;
                                    break;
                                }
                            }
                            if (!foundWord)
                            {
                                // If the word doesn't exist, add it with frequency 1
                                wordFreq.push_back({lowercaseWord, 1});
                            }
                        }
                    }
                    yearTitleWordFreq.push_back({releaseYear, wordFreq});
                }
            }
        }
    }

    return yearTitleWordFreq;
}

// Function to sort word frequencies for each year entry using quick sort
void sortWordFrequenciesByYear(vector<pair<int, vector<WordFrequency>>> &yearTitleWordFreq)
{
    for (auto &yearEntry : yearTitleWordFreq)
    {
        mergeSort(yearEntry.second, 0, yearEntry.second.size() - 1);
    }
}

void displayTopWordsByYear(vector<pair<int, vector<WordFrequency>>> &yearTitleWordFreq, int numYears)
{
    int yearsDisplayed = 0;

    // Sort the years in descending order
    mergeSortInt(yearTitleWordFreq, 0, yearTitleWordFreq.size() - 1);

    cout << "Top 10 words in movie titles segregated by year:" << endl;
    for (const auto &yearEntry : yearTitleWordFreq)
    {
        cout << "Year: " << yearEntry.first << endl;
        int count = 0;
        for (const auto &wf : yearEntry.second)
        {
            cout << wf.word << ": " << wf.frequency << " occurrences" << endl;
            count++;
            if (count >= 10)
                break;
        }
        cout << "--------------------------" << endl;

        yearsDisplayed++;
        if (yearsDisplayed >= numYears)
        {
            break; // Stop displaying years after the specified number of years
        }
    }
}

void displayTopWords(const vector<WordFrequency> &wordFreq, int limit)
{
    cout << "Top " << limit << " most common words in movie titles:" << endl;
    int count = 0;
    for (const auto &wf : wordFreq)
    {
        cout << wf.word << ": " << wf.frequency << " occurrences" << endl;
        count++;
        if (count >= limit)
            break;
    }
}

void displayWordFreqByLanguage(const vector<vector<WordFrequency>> &titleWordFreqByLanguage, int limit)
{
    cout << "Word frequencies in movie taglines according to original language (excluding ignored languages):" << endl;
    for (const auto &languageEntry : titleWordFreqByLanguage)
    {
        cout << "Original Language: " << languageEntry[0].word << endl;
        for (size_t i = 1; i < min(languageEntry.size(), static_cast<size_t>(limit + 1)); i++) // Display top 'limit' words
        {
            cout << languageEntry[i].word << ": " << languageEntry[i].frequency << endl;
        }
        cout << "--------------------------" << endl;
    }
}

void displayTopProductionCompanies(const vector<CompanyInfo> &companies)
{
    cout << "Top 10 Production Companies by Total Revenue:\n";
    int count = 0;
    for (const CompanyInfo &company : companies)
    {
        if (count >= 10)
            break;
        cout << company.name << " - Total Revenue: $" << company.totalRevenue << ", Produced Countries: ";
        for (size_t i = 0; i < company.producedCountries.size(); ++i)
        {
            cout << company.producedCountries[i];
            if (i != company.producedCountries.size() - 1)
            {
                cout << ", ";
            }
        }
        cout << endl;
        count++;
    }
}

void printTopMoviesByRevenue(const vector<Movie> &movies, int limit)
{
    cout << "Top " << limit << " Movie Titles with Highest Revenue and Their Production Companies:\n";
    int count = 0;
    for (const Movie &movie : movies)
    {
        if (count >= limit)
            break;
        cout << "Title: " << movie.title << " - Revenue: $" << movie.revenue << endl;
        cout << "Production Company: ";
        for (const string &company : movie.production_companies)
        {
            cout << company << ", ";
        }
        cout << endl
             << endl;
        count++;
    }
}

void printTopMoviesByPopularity(const vector<Movie> &movies, int limit)
{
    cout << "Top " << limit << " Movie Titles with Highest Popularity and Their Production Companies:\n";
    int count = 0;
    for (const Movie &movie : movies)
    {
        if (count >= limit)
            break;
        cout << "Title: " << movie.title << " - Popularity: $" << movie.popularity << endl;
        cout << "Production Company: ";
        for (const string &company : movie.production_companies)
        {
            cout << company << ", ";
        }
        cout << endl
             << endl;
        count++;
    }
}

void languageDistribution(const vector<Movie> &movies)
{
    vector<string> languages;
    vector<int> languageCount;

    // Iterate through the movies to count language frequency
    for (const auto &movie : movies)
    {
        bool languageFound = false;
        for (size_t i = 0; i < languages.size(); ++i)
        {
            if (movie.original_language == languages[i])
            {
                languageCount[i]++;
                languageFound = true;
                break;
            }
        }
        if (!languageFound)
        {
            languages.push_back(movie.original_language);
            languageCount.push_back(1);
        }
    }

    // Display language distribution
    cout << "Language Distribution:" << endl;
    for (size_t i = 0; i < languages.size(); ++i)
    {
        cout << languages[i] << ": " << languageCount[i] << " movies \n";
    }
}

int main()
{
    // Vector to define which columns need to have the double quotes removed and which do not
    vector<bool> remove_quotes = {false, false, true, true, false, false, true, true, false, false, true, false, false, false, false, false, false, false, false, false, false, false};
    vector<Movie> movies = parseCSV("animated_movies.csv", remove_quotes);
    // Words to ignore while doing word frequency analysis
    vector<string> ignoredWords = {"&", "-", "1", "2", "3", "a", "about", "al", "all", "an", "and", "animation", "as", "at", "au", "b", "d", "da", "das", "de", "dei", "del", "della", "der", "des", "di", "die", "do", "du", "e", "el", "elle", "en", "entre", "et", "f", "for", "from", "g", "gli", "go", "have", "how", "i", "il", "in", "is", "it", "k", "l", "la", "las", "le", "les", "los", "m", "movie", "my", "ni", "no", "o", "of", "on", "one", "os", "r", "seven", "t", "the", "there", "to", "un", "una", "und", "v", "ve", "was", "what", "who", "with", "y", "you", "your", "z"};
    // Vector of original languages to be ignored
    vector<string>
        ignoredLanguages = {"oc", "ab", "id", "mr", "la", "ar", "ms", "pa", "sk", "el", "bs", "ga", "tl", "cn", "kk", "is", "si", "st", "mo", "hy", "th", "as", "gl", "ur", "sw", "mn", "be", "he", "nb", "iu", "zu", "lt", "qu", "ta", "vi", "yi", "ca", "af", "ro", "ml", "uz", "cy", "mi", "ht", "sr", "ka", "fa", "eu", "cr", "kk", "ha", "bn", "dz", "nn", "os", "te", "bg", "mk", "sq", "si", "sl", "gn", "pa", "ku", "sa"};
    // Find the total revenue for each production company
    vector<CompanyInfo> companies = processCompanyInfo(movies);

    // Sort companies based on total revenue
    mergeSortCompanies(companies);

    // Display rge top production companies by revenue
    displayTopProductionCompanies(companies);

    // Find the country with the highest revenue
    string countryWithHighestRevenue = findCountryWithHighestProperty(movies, &Movie::revenue);
    cout << "Country with the highest revenue: " << countryWithHighestRevenue << endl;

    // Find the country with the highest IMDb rating
    string countryWithHighestRating = findCountryWithHighestProperty(movies, &Movie::vote_average);
    cout << "Country with the highest IMDb rating: " << countryWithHighestRating << endl;

    // Find the country with the highest popularity
    string countryWithHighestPopularity = findCountryWithHighestProperty(movies, &Movie::popularity);
    cout << "Country with the highest popularity: " << countryWithHighestPopularity << endl;

    // Find the country with the most number of movies
    string mostProducingCountry = findMostProducingCountry(movies);
    cout << "Country with the highest total number of movies produced: " << mostProducingCountry << endl;

    cout << "\n"
         << endl;
    countAllGenresFrequency(movies);
    cout << "\n"
         << endl;
    countReleaseYearFrequency(movies);
    cout << "\n"
         << endl;

    // Extract data for analysis
    vector<float> imdb_ratings;
    vector<int> runtimes;
    vector<long long> revenues;
    vector<float> popularities;
    vector<string> genres;
    vector<long long> budget;

    for (const auto &movie : movies)
    {
        imdb_ratings.push_back(movie.vote_average);
        revenues.push_back(movie.revenue);
        budget.push_back(movie.budget);
        popularities.push_back(movie.popularity);
        genres.push_back(movie.genres.empty() ? "" : movie.genres[0]); // Assuming only one genre per movie for simplicity
    }

    // Compute correlation coefficients
    cout << "Correlation between runtime and IMDb rating: " << endl;
    CorrelationResult runtime_imdb = correlation(runtimes, imdb_ratings);
    cout << "Correlation between runtime and popularity: " << endl;
    CorrelationResult runtime_popularities = correlation(runtimes, popularities);
    cout << "Correlation between runtime and revenue: " << endl;
    CorrelationResult runtime_revenues = correlation(runtimes, revenues);
    cout << "Correlation between revenue and popularity: " << endl;
    CorrelationResult revenue_popularity = correlation(revenues, popularities);
    cout << "Correlation between imdb_rating and popularity: " << endl;
    CorrelationResult imdb_rating_popularities = correlation(imdb_ratings, popularities);
    cout << "Correlation between revenue and imdb_rating: " << endl;
    CorrelationResult revenue_imdb_ratings = correlation(revenues, imdb_ratings);
    cout << "Correlation between budget and revenue: " << endl;
    CorrelationResult budget_revenue = correlation(budget, revenues);
    cout << "Correlation between budget and runtime: " << endl;
    CorrelationResult budget_runtime = correlation(budget, runtimes);
    cout << "Correlation between budget and popularity: " << endl;
    CorrelationResult budget_popularity = correlation(budget, popularities);
    cout << "Correlation between budget and imdb_ratings: " << endl;
    CorrelationResult budget_imdb_rating = correlation(budget, imdb_ratings);
    cout << "\n"
         << endl;

    // Output detailed information about correlations
    cout << "Correlation between runtime and IMDb rating: " << runtime_imdb.correlation_coefficient << endl;
    cout << "Correlation between runtime and popularity: " << runtime_popularities.correlation_coefficient << endl;
    cout << "Correlation between runtime and revenue: " << runtime_revenues.correlation_coefficient << endl;
    cout << "Correlation between revenue and popularity: " << revenue_popularity.correlation_coefficient << endl;
    cout << "Correlation between revenue and imdb_rating: " << revenue_imdb_ratings.correlation_coefficient << endl;
    cout << "Correlation between imdb_rating and popularity: " << imdb_rating_popularities.correlation_coefficient << endl;
    cout << "Correlation between budget and revenue: " << budget_revenue.correlation_coefficient << endl;
    cout << "Correlation between budget and runtime: " << budget_runtime.correlation_coefficient << endl;
    cout << "Correlation between budget and popularity: " << budget_popularity.correlation_coefficient << endl;
    cout << "Correlation between budget and imdb_ratings: " << budget_imdb_rating.correlation_coefficient << endl;

    // Displaying language dstribution
    languageDistribution(movies);

    // Count word frequencies in movie titles
    vector<WordFrequency> wordFreq = countWords(movies, ignoredWords);

    // Sorting the word frequencies using Merge Sort
    mergeSort(wordFreq, 0, wordFreq.size() - 1);

    // Display the top 30 most common words
    displayTopWords(wordFreq, 30);

    // Count word frequencies in movie taglines according to original language, excluding ignored languages
    vector<vector<WordFrequency>> titleWordFreqByLanguage = countTitleWordsByOriginalLanguage(movies, ignoredLanguages, ignoredWords);

    // Sort word frequencies for each language entry using quick sort
    for (auto &languageEntry : titleWordFreqByLanguage)
    {
        mergeSort(languageEntry, 1, languageEntry.size() - 1);
    }

    // Display word frequencies for each original language
    displayWordFreqByLanguage(titleWordFreqByLanguage, 5);
    // Display top 5 words for each language

    // Count word frequencies in movie titles segregated by year
    vector<pair<int, vector<WordFrequency>>> yearTitleWordFreq = countTitleWordsByYear(movies, ignoredWords);

    // Sort word frequencies for each year entry
    sortWordFrequenciesByYear(yearTitleWordFreq);

    // Display top 5 words in titles segregated by year
    displayTopWordsByYear(yearTitleWordFreq, 25);

    // Sort movies by revenue using merge sort
    movies = mergeSortMovie(movies, "revenue");

    // Print the top 10 movies by revenue
    printTopMoviesByRevenue(movies, 10);

    string column = "popularity";
    movies = mergeSortMovie(movies, column);
    int count = 0;
    // Print the top 10 movies by popularity
    printTopMoviesByPopularity(movies, 10);

    return 0;
}
