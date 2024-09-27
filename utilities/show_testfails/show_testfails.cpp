/*
 * (C) F. J. Madrid Cuevas <fjmadrid@uco.es>

  Esta es una utilidad para usar en las prácticas de FSIV.
*/

#include <iostream>
#include <exception>
#include <vector>
#include <string>
#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>

const char * keys =
    "{help h usage ? |      | Print this message}"
    "{version v      |      | Print version number}"
    "{@input         | <none> | input test_fails filename.}"
    ;

void
print_version()
{
    std::cout << " version: 1.0" << std::endl;
}

void
show_node_details(const cv::FileNode& node)
{
    std::cout << "[" << node.name() << "]";
    switch (node.type())
    {
    case cv::FileNode::INT:
        std::cout << ":\t" << static_cast<int>(node.real()) << std::endl;
        break;
    case cv::FileNode::REAL:
        std::cout << ":\t" << node.real() << std::endl;
        break;
    case cv::FileNode::STRING:
        std::cout << ":\t'" << node.string() << '\'' << std::endl;
        break;
    case cv::FileNode::MAP:
        std::cout << ":\t matrix" << std::endl;
        break;
    case cv::FileNode::SEQ:
    {
        std::vector<float> data;
        node >> data;
        std::cout << ":\t[";
        for (const auto& v: data)
            std::cout << v << ", ";
        std::cout << "]" << std::endl;
        break;
    }
    default:
        std::cout << ":\t unknown type (" << int(node.type()) << ")." << std::endl;
    }
}

void
show_node_keys(const cv::FileNode& root_node)
{
    for (cv::FileNodeIterator fit = root_node.begin();
         fit != root_node.end(); ++fit)
    {
        cv::FileNode item = *fit;
        show_node_details(item);
    }
}

int
main (int argc, char* const* argv)
{
    int retCode=EXIT_SUCCESS;

    try {
        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Show data of a test fails file.");
        if (parser.has("help"))
        {
            parser.printMessage();
            return EXIT_SUCCESS;
        }
        if (parser.has("version"))
        {
            print_version();
            return EXIT_SUCCESS;
        }

        cv::String input_n = parser.get<cv::String>("@input");
        if (!parser.check())
        {
            parser.printErrors();
            return EXIT_FAILURE;
        }

        cv::FileStorage fs (input_n, cv::FileStorage::READ);
        if (!fs.isOpened())
        {
            std::cerr << "Error: could not open file ["
                      << input_n << "]." << std::endl;
            return EXIT_FAILURE;
        }

        auto root_node = fs.root();
        int option = 0;
        do
        {
            std::cout << "****************************************" << std::endl;
            std::cout << "* File keys:" << std::endl;
            std::cout << std::endl;
            show_node_keys(root_node);

            std::cout << std::endl;
            std::cout << "* Options:" << std::endl;
            std::cout << "* 0\t Quit." << std::endl;
            std::cout << "* 1\t Show a key as a matrix." << std::endl;
            std::cout << "* 2\t Show a key as a image."  << std::endl;
            std::cout << "****************************************" << std::endl;

            do
            {
                std::cout << "Option?: ";
                std::cin >> option; std::cin.ignore();
            } while(option<0 || option>2);

            if (option != 0)
            {
                std::string key;
                std::cout << "Get key name: ";
                std::cin >> key;
                cv::FileNode node = fs[key];
                if (node.empty())
                {
                    std::cerr << "Error: unknown key name [" << key << "].";
                }
                else
                {
                    cv::Mat mat = node.mat();
                    switch (option) {
                    case 1: {
                        std::cout << "Mat values:" << std::endl;
                        std::cout << mat << std::endl;
                        break;
                    }
                    case 2: {
                        cv::namedWindow(key, cv::WINDOW_GUI_EXPANDED | cv::WINDOW_FREERATIO);
                        cv::imshow(key, mat);
                        cv::resizeWindow(key, cv::Size(256, 256));
                        std::cout << "Press any key on the window to continue." << std::endl;
                        cv::waitKey(0);
                        break;
                    }
                    default: {
                        std::cout << "Unknon option '" << char(option) << std::endl;
                    }
                    }
                }
            }
        }
        while (option != 0);
    }
    catch (std::exception& e)
    {
        std::cerr << "Capturada excepcion: " << e.what() << std::endl;
        retCode = EXIT_FAILURE;
    }
    return retCode;
}
