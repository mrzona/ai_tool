#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

// Define the API endpoint 
const std::string API_ENDPOINT = "https://api.openai.com/v1/chat/completions";

// Function to send a POST request to the ChatGPT API
std::string sendChatGPTRequest(const std::string& apiKey, const std::string& question) {
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Construct the request JSON
        std::string json_data = "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"system\",\"content\":\"You are a helpful assistant.\"},{\"role\":\"user\",\"content\":\"" + question + "\"}]}";

        curl_easy_setopt(curl, CURLOPT_URL, API_ENDPOINT.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* buffer, size_t size, size_t nmemb, void* userp) -> size_t {
            size_t totalSize = size * nmemb;
            static_cast<std::string*>(userp)->append(static_cast<char*>(buffer), totalSize);
            return totalSize;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error during API request: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Parse the JSON response
            nlohmann::json jsonResponse;
            try {
                jsonResponse = nlohmann::json::parse(response);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
                return "";
            }

            // Check if there is an "error" field in the response
            if (jsonResponse.find("error") != jsonResponse.end()) {
                // Extract and print the error message
                std::string errorMessage = jsonResponse["error"]["message"];
                std::cout << "Error Message: " << errorMessage << std::endl;
            } else {
                // Print the JSON response directly
                std::cout << "Response:\n" << jsonResponse.dump(4) << std::endl;
            }
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return response;
    }
    return "";
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <API_KEY> <question>" << std::endl;
        return 1;
    }

    std::string apiKey = argv[1];
    std::string question = argv[2];

    std::string response = sendChatGPTRequest(apiKey, question);
    // You can parse and use the response as needed

    return 0;
}
