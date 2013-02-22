import sys
import os
import shutil
import random
import re

non_image_extensions = ['.txt','.csv','.mat']

def print_usage():
    print '''Select images out of a directory randomly to use as a test set
Usage: python pick_test_set.py DB_DIR OUT_DIR NUM_IMAGES'''

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
    if len(sys.argv) < 4:
        print_usage();
        sys.exit(1)

    db_dir = sys.argv[1]
    out_dir = sys.argv[2]
    num_images = int(sys.argv[3])

    dir_contents = os.listdir(db_dir)
    if len(dir_contents) < num_images:
        print 'ERROR: directory contains too few files'
        sys.exit(1)

    # remove non image files... with unreadable python fanciness :D
    images = filter(lambda x: False if True in [x.endswith(ext) for ext in non_image_extensions] else True, dir_contents)

    if len(images) < num_images:
        print 'ERROR: directory contains too few images'
        sys.exit(1)

    # get categories
    classes = get_classes(images)
    # select a few classes
    classes = random.sample(classes, 50)
    # take a few images from every class
    images_by_class = {}
    for class_name in classes:
        images_by_class[class_name] = filter(lambda x: x.startswith(class_name), images)
    sampled_images = []
    for class_name in classes:
        sampled_images += random.sample(images_by_class[class_name], 10)

    # randomly sample all images
    #sampled_images = random.sample(images, num_images)



    # copy files to OUT_DIR
    for im in sampled_images:
        srcpath = os.path.join(db_dir, im)
        destpath = os.path.join(out_dir, im)
        shutil.copyfile(srcpath,destpath)
        print 'Copied %s to %s' % (srcpath, destpath)

    print 'DONE'

if __name__ == '__main__':
    main()
