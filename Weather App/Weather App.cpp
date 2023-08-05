
#include <iostream>
#include <string>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>

using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::client;

double kelvin_to_celcius(double kelvin) {
    return (kelvin - 273.15);
}

int main()
{
    cout << "\t *******************************************************************\n";
    cout << "\t\t  My Weather App \n ";
    cout << "\t *******************************************************************\n";

    string city;
    cout << "\n Enter The Name of the City : ";
    getline(cin, city);

    http_client client(U("https://api.openweathermap.org/data/2.5"));
    uri_builder builder(U("/weather"));
    builder.append_query(U("q"), utility::conversions::to_string_t(city));
    builder.append_query(U("appid"), U("5dbb1ad75499fc23ea24751ce7a861ea"));
    http_request request(http::methods::GET);
    request.set_request_uri(builder.to_string());

    try
    {
        client.request(request).then([](http_response response) {
            if (response.status_code() == status_codes::OK) {
                return response.extract_json();
            }
            else if (response.status_code() == status_codes::NotFound) {
                cout << "City not found. Please check the city name and try again." << endl;
                return pplx::task_from_result(web::json::value());
            }
            else {
                cout << "HTTP request failed with status code: " << response.status_code() << endl;
                return pplx::task_from_result(web::json::value());
            }
            }).then([](web::json::value body) {
                if (!body.is_null()) {
                    if (body.has_field(U("main"))) {
                        cout << "\n Temperature: " << kelvin_to_celcius( body[U("main")][U("temp")].as_double()) << "\n";
                        double humidity = body[U("main")][U("humidity")].as_double();
                        cout << "\n Humidity: " << humidity << "%" << "\n";
                        cout << "\n Longitude Of City : " << body[U("coord")][U("lon")].as_double()<<"\n";
                        cout << "\n Latitude Of City : " << body[U("coord")][U("lat")].as_double()<< "\n";
                        cout << "\n Minimum Temperature : " << kelvin_to_celcius(body[U("main")][U("temp_min")].as_double()) << "\n";
                        cout << "\n Maximum Temperature : " << kelvin_to_celcius(body [U("main")][U("temp_max")].as_double()) << "\n";
                        cout << "\n Atmosphere Pressure :  " << body[U("main")][U("pressure")].as_double() << " Pascal \n";
                        cout << "\n Wind Speed :  " << body[U("wind")][U("speed")].as_double() << " kmph \n";

                    }
                    else {
                        cout << "JSON parsing error: 'main' field not found in the response." << endl;
                    }
                }
                }).wait();
    }
    catch (const web::json::json_exception& e)
    {
        cout << "JSON parsing error: " << e.what() << endl;
    }
    catch (const std::exception& e)
    {
        cout << "An error occurred: " << e.what() << endl;
    }

    return 0;
}
