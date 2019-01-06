local datasets = {
    iris = {
        url = "https://raw.githubusercontent.com/uiuc-cse/data-fa14/gh-pages/data/iris.csv",
        author = "Created by: R.A. Fisher, Donor: Michael Marshall",
        about = "The Iris flower data set or Fisher's Iris data set is a multivariate data set introduced by the British statistician and biologist Ronald Fisher in his 1936 paper The use of multiple measurements in taxonomic problems as an example of linear discriminant analysis. It is sometimes called Anderson's Iris data set because Edgar Anderson collected the data to quantify the morphologic variation of Iris flowers of three related species. Two of the three species were collected in the Gaspé Peninsula \"all from the same pasture, and picked on the same day and measured at the same time by the same person with the same apparatus\".\n\nThe data set consists of 50 samples from each of three species of Iris (Iris setosa, Iris virginica and Iris versicolor). Four features were measured from each sample: the length and the width of the sepals and petals, in centimeters. Based on the combination of these four features, Fisher developed a linear discriminant model to distinguish the species from each other. (Source, Wikipedia: https://en.wikipedia.org/wiki/Iris_flower_data_set)",
        website = "https://archive.ics.uci.edu/ml/datasets/iris"
    }
} 

function datasets.download(key, path)
    local https = require "ssl.https" --luasec

    print("Downloading `".. key.."` dataset into `"..path.."`.")
    local r,c,h,s = https.request {
        url = datasets[key].url,
        --sink = ltn12.sink.chain(printStatus, ltn12.sink.file(io.open(path, "wb")))
        sink = ltn12.sink.file(io.open(path, "wb"))
    }

end

return datasets