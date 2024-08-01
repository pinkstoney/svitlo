#include <svitlo/data-fetcher.h>

#include <iostream>

int main()
{
    try
    {
        DataFetcher fetcher;
        fetcher.setPostData("accountNumber", "25012345");
        std::string response = fetcher.send();
        std::cout << "Response received: " << response << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
