#include "exiv2/image.hpp"
#include "ruby.h"

// Create a Ruby string from a C++ std::string.
static VALUE to_ruby_string(const std::string& string) {
  return rb_str_new(string.data(), string.length());
}

// Create a C++ std::string from a Ruby string.
static std::string to_std_string(VALUE string) {
  return std::string(RSTRING(string)->ptr, RSTRING(string)->len);
}

// Extract data from an Exiv2::IptcData or an Exiv2::ExifData into a Ruby hash.
template <class T>
static VALUE data_to_hash(T& data) {
  VALUE hash = rb_hash_new();

  for(typename T::iterator i = data.begin(); i != data.end(); i++) {
    VALUE key   = to_ruby_string(i->key());
    VALUE value = to_ruby_string(i->value().toString());
    rb_hash_aset(hash, key, value);
  }

  return hash;
}

extern "C" {
  typedef VALUE (*Method)(...);

  static VALUE exiv2_module;

  static VALUE basic_error_class;

  static VALUE image_class;
  static void image_free(Exiv2::Image* image);
  static VALUE image_read_metadata(VALUE self);
  static VALUE image_iptc_data(VALUE self);
  static VALUE image_exif_data(VALUE self);

  static VALUE image_factory_class;
  static VALUE image_factory_open(VALUE klass, VALUE path);


  void Init_exiv2() {
    exiv2_module = rb_define_module("Exiv2");

    basic_error_class = rb_define_class_under(exiv2_module, "BasicError", rb_eRuntimeError);

    image_class = rb_define_class_under(exiv2_module, "Image", rb_cObject);
    rb_define_method(image_class, "read_metadata", (Method)image_read_metadata, 0);
    rb_define_method(image_class, "iptc_data", (Method)image_iptc_data, 0);
    rb_define_method(image_class, "exif_data", (Method)image_exif_data, 0);

    image_factory_class = rb_define_class_under(exiv2_module, "ImageFactory", rb_cObject);
    rb_define_singleton_method(image_factory_class, "open", (Method)image_factory_open, 1);
  }

  
  // Exiv2::Image Methods
  
  static void image_free(Exiv2::Image* image) {
    delete image;
  }
  
  static VALUE image_read_metadata(VALUE self) {
    Exiv2::Image* image;
    Data_Get_Struct(self, Exiv2::Image, image);

    try {
      image->readMetadata();
    }
    catch (Exiv2::BasicError<char> error) {
      rb_raise(basic_error_class, "%s", error.what());
    }

    return Qnil;
  }

  static VALUE image_exif_data(VALUE self) {
    Exiv2::Image* image;
    Data_Get_Struct(self, Exiv2::Image, image);

    return data_to_hash<Exiv2::ExifData>(image->exifData());
  }

  static VALUE image_iptc_data(VALUE self) {
    Exiv2::Image* image;
    Data_Get_Struct(self, Exiv2::Image, image);

    return data_to_hash<Exiv2::IptcData>(image->iptcData());
  }


  // Exiv2::ImageFactory methods

  static VALUE image_factory_open(VALUE klass, VALUE path) {
    Exiv2::Image* image;

    try {
      Exiv2::Image::AutoPtr image_auto_ptr = Exiv2::ImageFactory::open(to_std_string(path));
      image = image_auto_ptr.release(); // Release the AutoPtr, so we can keep the image around.
    }
    catch (Exiv2::BasicError<char> error) {
      rb_raise(basic_error_class, "%s", error.what());
    }
  
    return Data_Wrap_Struct(image_class, 0, image_free, image);
  }

}
