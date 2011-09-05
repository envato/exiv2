#include "exiv2/image.hpp"
#include "ruby.h"

// Create a Ruby string from a C++ std::string.
static VALUE to_ruby_string(const std::string& string) {
  return rb_str_new(string.data(), string.length());
}

// Create a C++ std::string from a Ruby string.
static std::string to_std_string(VALUE string) {
  string = StringValue(string); // Convert the Ruby object to a string if it isn't one.
  return std::string(RSTRING_PTR(string), RSTRING_LEN(string));
}

// Shared method for implementing each on XmpData, IptcData and ExifData.
template <class T>
static VALUE metadata_each(VALUE self) {
  T* data;
  Data_Get_Struct(self, T, data);

  for(typename T::iterator i = data->begin(); i != data->end(); i++) {
    VALUE key   = to_ruby_string(i->key());
    VALUE value = to_ruby_string(i->value().toString());
    rb_yield(rb_ary_new3(2, key, value));
  }

  return Qnil;
}


extern "C" {
  typedef VALUE (*Method)(...);

  static VALUE exiv2_module;

  static VALUE basic_error_class;

  static VALUE image_class;
  static void image_free(Exiv2::Image* image);
  static VALUE image_read_metadata(VALUE self);
  static VALUE image_iptc_data(VALUE self);
  static VALUE image_xmp_data(VALUE self);
  static VALUE image_exif_data(VALUE self);

  static VALUE image_factory_class;
  static VALUE image_factory_open(VALUE klass, VALUE path);

  static VALUE exif_data_class;
  static VALUE exif_data_each(VALUE self);

  static VALUE iptc_data_class;
  static VALUE iptc_data_each(VALUE self);

  static VALUE xmp_data_class;
  static VALUE xmp_data_each(VALUE self);

  void Init_exiv2() {
    VALUE enumerable_module = rb_const_get(rb_cObject, rb_intern("Enumerable"));

    exiv2_module = rb_define_module("Exiv2");

    basic_error_class = rb_define_class_under(exiv2_module, "BasicError", rb_eRuntimeError);

    image_class = rb_define_class_under(exiv2_module, "Image", rb_cObject);
    rb_define_method(image_class, "read_metadata", (Method)image_read_metadata, 0);
    rb_define_method(image_class, "iptc_data", (Method)image_iptc_data, 0);
    rb_define_method(image_class, "xmp_data", (Method)image_xmp_data, 0);
    rb_define_method(image_class, "exif_data", (Method)image_exif_data, 0);

    image_factory_class = rb_define_class_under(exiv2_module, "ImageFactory", rb_cObject);
    rb_define_singleton_method(image_factory_class, "open", (Method)image_factory_open, 1);

    exif_data_class = rb_define_class_under(exiv2_module, "ExifData", rb_cObject);
    rb_include_module(exif_data_class, enumerable_module);
    rb_define_method(exif_data_class, "each", (Method)exif_data_each, 0);

    iptc_data_class = rb_define_class_under(exiv2_module, "IptcData", rb_cObject);
    rb_include_module(iptc_data_class, enumerable_module);
    rb_define_method(iptc_data_class, "each", (Method)iptc_data_each, 0);
    
    xmp_data_class = rb_define_class_under(exiv2_module, "XmpData", rb_cObject);
    rb_include_module(xmp_data_class, enumerable_module);
    rb_define_method(xmp_data_class, "each", (Method)xmp_data_each, 0);
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

    VALUE exif_data = Data_Wrap_Struct(exif_data_class, 0, 0, &image->exifData());
    rb_iv_set(exif_data, "@image", self);  // Make sure we don't GC the image until there are no references to the EXIF data left.

    return exif_data;
  }

  static VALUE image_iptc_data(VALUE self) {
    Exiv2::Image* image;
    Data_Get_Struct(self, Exiv2::Image, image);

    VALUE iptc_data = Data_Wrap_Struct(iptc_data_class, 0, 0, &image->iptcData());
    rb_iv_set(iptc_data, "@image", self);  // Make sure we don't GC the image until there are no references to the IPTC data left.

    return iptc_data;
  }


  static VALUE image_xmp_data(VALUE self) {
    Exiv2::Image* image;
    Data_Get_Struct(self, Exiv2::Image, image);

    VALUE xmp_data = Data_Wrap_Struct(xmp_data_class, 0, 0, &image->xmpData());
    rb_iv_set(xmp_data, "@image", self);  // Make sure we don't GC the image until there are no references to the XMP data left.

    return xmp_data;
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


  // Exiv2::ExifData methods

  static VALUE exif_data_each(VALUE self) {
    return metadata_each<Exiv2::ExifData>(self);
  }
  

  // Exiv2::IptcData methods
  
  static VALUE iptc_data_each(VALUE self) {
    return metadata_each<Exiv2::IptcData>(self);
  }
 
  // Exiv2::XmpData methods
  
  static VALUE xmp_data_each(VALUE self) {
    return metadata_each<Exiv2::XmpData>(self);
  } 
}
