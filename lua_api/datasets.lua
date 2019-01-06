
local https = require "ssl.https" --luasec

local datasets = {
    iris = {
        url = "https://raw.githubusercontent.com/uiuc-cse/data-fa14/gh-pages/data/iris.csv",
        author = "Created by: R.A. Fisher, Donor: Michael Marshall",
        about = "The Iris flower data set or Fisher's Iris data set is a multivariate data set introduced by the British statistician and biologist Ronald Fisher in his 1936 paper The use of multiple measurements in taxonomic problems as an example of linear discriminant analysis. It is sometimes called Anderson's Iris data set because Edgar Anderson collected the data to quantify the morphologic variation of Iris flowers of three related species. Two of the three species were collected in the Gaspé Peninsula \"all from the same pasture, and picked on the same day and measured at the same time by the same person with the same apparatus\".\n\nThe data set consists of 50 samples from each of three species of Iris (Iris setosa, Iris virginica and Iris versicolor). Four features were measured from each sample: the length and the width of the sepals and petals, in centimeters. Based on the combination of these four features, Fisher developed a linear discriminant model to distinguish the species from each other. (Source, Wikipedia: https://en.wikipedia.org/wiki/Iris_flower_data_set)",
        website = "https://archive.ics.uci.edu/ml/datasets/iris",
    },

    mnist = {
        url = {"https://pjreddie.com/media/files/mnist_train.csv", "https://pjreddie.com/media/files/mnist_test.csv"},
        author = "Yann LeCun, Courant Institute, NYU & Corinna Cortes, Google Labs, New York & Christopher J.C. Burges, Microsoft Research, Redmond",
        about = "The MNIST database (Modified National Institute of Standards and Technology database) is a large database of handwritten digits that is commonly used for training various image processing systems.The database is also widely used for training and testing in the field of machine learning. It was created by \"re-mixing\" the samples from NIST's original datasets. The creators felt that since NIST's training dataset was taken from American Census Bureau employees, while the testing dataset was taken from American high school students, it was not well-suited for machine learning experiments. Furthermore, the black and white images from NIST were normalized to fit into a 28x28 pixel bounding box and anti-aliased, which introduced grayscale levels.\n\nThe MNIST database contains 60,000 training images and 10,000 testing images. Half of the training set and half of the test set were taken from NIST's training dataset, while the other half of the training set and the other half of the test set were taken from NIST's testing dataset. There have been a number of scientific papers on attempts to achieve the lowest error rate; one paper, using a hierarchical system of convolutional neural networks, manages to get an error rate on the MNIST database of 0.23%.The original creators of the database keep a list of some of the methods tested on it. In their original paper, they use a support vector machine to get an error rate of 0.8%. An extended dataset similar to MNIST called EMNIST has been published in 2017, which contains 240,000 training images, and 40,000 testing images of handwritten digits and characters.",
        website = "http://yann.lecun.com/exdb/mnist/",
    }
}



datasets.iris.download = function(folder)
    local key = datasets.iris.url
    local fullpath = folder.."/iris.csv"
    print("Downloading `".. key.."` dataset into `"..fullpath.."`.")
    local r,c,h,s = https.request {
        url = key,
        --sink = ltn12.sink.chain(printStatus, ltn12.sink.file(io.open(path, "wb")))
        sink = ltn12.sink.file(io.open(fullpath, "wb"))
    }
end

datasets.mnist.download = function(folder)
    local train = datasets.mnist.url[1]
    local test = datasets.mnist.url[2]
    local fullpath_train = folder.."/train.csv"
    local fullpath_test = folder.."/test.csv"
    print("Downloading `".. train.."` dataset into `"..fullpath_train.."`.")
    local r,c,h,s = https.request {
        url = train,
        sink = ltn12.sink.file(io.open(fullpath_train, "wb"))
    }
    print("Downloading `".. test.."` dataset into `"..fullpath_test.."`.")
    local r,c,h,s = https.request {
        url = test,
        sink = ltn12.sink.file(io.open(fullpath_test, "wb"))
    }
end

return datasets