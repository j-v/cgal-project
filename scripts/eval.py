import math
import sys
import re

DEFAULT_N = 5
DEFAULT_T = 20.0

def printusage():
    print '''eval.py: Evaluate the shape matching system.
usage: python eval.py EMD_INDEX_FILE EMD_MAT_FILE [N [T]]
N: Neighborhood size (default %d)
T: EMD Threshold (default %f)
''' % (DEFAULT_N, DEFAULT_T)
def load_mat(filename):
    f = open(filename)
    mat = []
    for l in f.readlines():
        mat.append(list(float(n) for n in l.split(' ')))
    return mat


def load_lib(filename):
    f = open(filename)
    img_names = []
    for l in f.readlines():
        img_names.append(list(n for n in l.split(','))[0])
    f.close()
    return img_names

def get_class_name(img_name):
    m = re.search("\d", img_name)
    if m == None:
        return None
    digit_index = m.start()
    class_name = img_name[:digit_index]
    return class_name

def get_classes(img_list):
    classes = set()
    for im in img_list:
        class_name = get_class_name(im)
        if class_name == None:
            print 'WARNING: filename %s invalid' % im
            continue
        classes.add(class_name)
    return classes


def main():
    if len(sys.argv) < 3:
        printusage()
        sys.exit(1)
    emd_index_file = sys.argv[1]
    emd_mat_file = sys.argv[2]

    # all items in neighbourhood must have distance less than threshold
    max_neighborhood_size = DEFAULT_N if len(sys.argv) < 4 else int(sys.argv[3])
    threshold = DEFAULT_T if len(sys.argv) < 5 else float(sys.argv[4])

    images = load_lib(emd_index_file)
    # test: print image filenames
    #for im in images: print im
    print 'Number of images in db: %d' % (len(images))
    classes = get_classes(images)
    print 'Number of classes: %d' % len(classes)
    dist_mat = load_mat(emd_mat_file)
    mat_N = len(dist_mat)
    mat_M = len(dist_mat[0])
    print 'Matrix dims: %d x %d' % (mat_N, mat_M)

    # test: print class names
    #for class_name in classes: print class_name

    # Do evaluation
    # 1. compile all scores with image names
    all_scores = {}
    for i in range(mat_N):
        im_name = images[i]
        class_name = get_class_name(im_name)
        scores = []
        for j in range(mat_N):
            if i==j: continue
            scores.append( (dist_mat[i][j], images[j]) )
        all_scores[im_name] = sorted(scores)

    # test: print top N matches of each image
    #for im_name,scores in all_scores.items():
    #    print '%s : %s' % (im_name, str(scores[:5]))


    eval_scores = {}
    classifications = {}
    for im_name, scores in all_scores.items():
        neighborhood = []
        for score, other_im_name in scores[:max_neighborhood_size]:
            if score > threshold: break
            neighborhood.append(other_im_name)
        # test: print out neighborhood
        #print "%s: %s" % (im_name, str(neighborhood))

        # eval_score = (num matching classes in neighborhood) / (neighborhood size)
        n_classes = [get_class_name(im) for im in neighborhood]
        if len(neighborhood) == 0:
            eval_score = 0
        else:
            matching_classes = n_classes.count(get_class_name(im_name))
            eval_score = float(matching_classes) / float(len(neighborhood))
        eval_scores[im_name] = eval_score

        # classify object using neighbor class count
        if len(n_classes) > 0:
            class_scores = []
            for class_name in set(n_classes):
                class_scores.append( (n_classes.count(class_name), class_name) )
            class_scores = sorted(class_scores, reverse=True)
            class_guess = class_scores[0][1]
        else:
            class_guess = None
        classifications[im_name] = class_guess

        #test print classification
        # print '%s: %s' % (im_name, classifications[im_name])

    # get overall score
    if len(eval_scores) > 0:
        average_score = sum(eval_scores.values()) / len(eval_scores.values())
    else:
        average_score = 0

    # get number of correct classifications
    correct_guesses = 0
    for im_name, im_class in classifications.items():
        if get_class_name(im_name) == im_class: correct_guesses += 1
    print 'Correct classifications:', correct_guesses
    print 'Total images:', mat_N
    classifier_score = correct_guesses / float(mat_N)
    print 'classifier score:', classifier_score

    print 'Average score', average_score

    # test get number of correct classifications per class
    class_counts = {}
    class_correct = {}
    for class_name in classes:
        class_counts[class_name] = 0
        class_correct[class_name] = 0
    for im_name, im_class in classifications.items():
        correct_class = get_class_name(im_name)
        class_counts[correct_class] += 1
        if im_class == None: continue
        guess_correct = correct_class == im_class
        if guess_correct: class_correct[correct_class] += 1
    for class_name in sorted(classes):
        print class_name, ': ', class_correct[class_name], ' / ', class_counts[class_name]





if __name__ == '__main__':
    main()
