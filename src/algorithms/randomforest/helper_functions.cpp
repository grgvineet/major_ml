void algo::randomforest::read_data_from_csv(std::string filename, cv::Mat data, cv::Mat classes, std::uint64_t num_samples, std::uint64_t num_attributes)
{
    float tmp;
    std::string str,var;
    std::string::size_type sz;     // alias of size_t
    std::ifstream myfile( filename );
    if( !myfile )
    {
        printf("ERROR: cannot read file %s\n",  filename.c_str());
        return ; // all not OK
    }

    for(int line = 0; line < num_samples; line++)
    {
        std::getline(myfile, str);
        std::istringstream ss(str);

        for(int attribute = 0; attribute < num_attributes + 1; attribute++)
        {
            if (attribute < num_attributes)
            {
                std::getline(ss, var, ',');
                data.at<float>(line, attribute) = std::stof (var,&sz);
            }
            else if (attribute == num_attributes)
            {
                std::getline(ss, var, ',');
                classes.at<float>(line, 0) =  std::stof (var,&sz);
            }
        }
    }
    return ; // all OK
}

std::size_t  algo::randomforest::locidx(std::size_t i, std::size_t np, std::size_t nl)
{
    return i / (np/nl);
}