#include "exiv2/image.hpp"
#include "exiv2/error.hpp"
#include "ruby.h"

// Create a Ruby string from a C++ std::string.
static VALUE to_ruby_string(const std::string& string) {
  VALUE str = rb_str_new(string.data(), string.length());

  // force UTF-8 encoding in Ruby 1.9+
  ID forceEncodingId = rb_intern("force_encoding");
  if(rb_respond_to(str, forceEncodingId)) {
      rb_funcall(str, forceEncodingId, 1, rb_str_new("UTF-8", 5));
  }
  return str;
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

typedef VALUE (*Method)(...);

static VALUE exiv2_module;

static VALUE basic_error_class;

static VALUE image_class;
static void image_free(Exiv2::Image* image);
static VALUE image_read_metadata(VALUE self);
static VALUE image_write_metadata(VALUE self);
static VALUE image_iptc_data(VALUE self);
static VALUE image_xmp_data(VALUE self);
static VALUE image_exif_data(VALUE self);

static VALUE image_factory_class;
static VALUE image_factory_open(VALUE klass, VALUE path);

static VALUE exif_data_class;
static VALUE exif_data_each(VALUE self);
static VALUE exif_data_add(VALUE self, VALUE key, VALUE value);
static VALUE exif_data_delete(VALUE self, VALUE key);

static VALUE iptc_data_class;
static VALUE iptc_data_each(VALUE self);
static VALUE iptc_data_add(VALUE self, VALUE key, VALUE value);
static VALUE iptc_data_delete(VALUE self, VALUE key);

static VALUE xmp_data_class;
static VALUE xmp_data_each(VALUE self);
static VALUE xmp_data_add(VALUE self, VALUE key, VALUE value);
static VALUE xmp_data_delete(VALUE self, VALUE key);
static VALUE xmp_register_ns(VALUE self, VALUE uri, VALUE ns);

extern "C" void Init_exiv2() {
  VALUE enumerable_module = rb_const_get(rb_cObject, rb_intern("Enumerable"));

  exiv2_module = rb_define_module("Exiv2");

  basic_error_class = rb_define_class_under(exiv2_module, "BasicError", rb_eRuntimeError);

  image_class = rb_define_class_under(exiv2_module, "Image", rb_cObject);
  rb_define_method(image_class, "read_metadata", (Method)image_read_metadata, 0);
  rb_define_method(image_class, "write_metadata", (Method)image_write_metadata, 0);
  rb_define_method(image_class, "iptc_data", (Method)image_iptc_data, 0);
  rb_define_method(image_class, "xmp_data", (Method)image_xmp_data, 0);
  rb_define_method(image_class, "exif_data", (Method)image_exif_data, 0);

  image_factory_class = rb_define_class_under(exiv2_module, "ImageFactory", rb_cObject);
  rb_define_singleton_method(image_factory_class, "open", (Method)image_factory_open, 1);

  exif_data_class = rb_define_class_under(exiv2_module, "ExifData", rb_cObject);
  rb_include_module(exif_data_class, enumerable_module);
  rb_define_method(exif_data_class, "each", (Method)exif_data_each, 0);
  rb_define_method(exif_data_class, "add", (Method)exif_data_add, 2);
  rb_define_method(exif_data_class, "delete", (Method)exif_data_delete, 1);

  iptc_data_class = rb_define_class_under(exiv2_module, "IptcData", rb_cObject);
  rb_include_module(iptc_data_class, enumerable_module);
  rb_define_method(iptc_data_class, "each", (Method)iptc_data_each, 0);
  rb_define_method(iptc_data_class, "add", (Method)iptc_data_add, 2);
  rb_define_method(iptc_data_class, "delete", (Method)iptc_data_delete, 1);
  
  xmp_data_class = rb_define_class_under(exiv2_module, "XmpData", rb_cObject);
  rb_include_module(xmp_data_class, enumerable_module);
  rb_define_method(xmp_data_class, "each", (Method)xmp_data_each, 0);
  rb_define_method(xmp_data_class, "add", (Method)xmp_data_add, 2);
  rb_define_method(xmp_data_class, "delete", (Method)xmp_data_delete, 1);
  rb_define_method(xmp_data_class, "register", (Method)xmp_register_ns, 2);
}


// Exiv2::Image Methods

static void image_free(Exiv2::Image* image) {
  delete image;
}

static VALUE image_read_metadata(VALUE self) {
  VALUE result = Qnil;
  Exiv2::Image* image;
  Data_Get_Struct(self, Exiv2::Image, image);

  try {
    char buf[BUFSIZ];
    setbuf(stderr, buf);
    image->readMetadata();
    if (buf[1]) {
      result = rb_sprintf("%s", buf); // If stderr outputs, we direct that into a string and return as result
    }
  }
  catch (Exiv2::BasicError<char> error) {
    rb_raise(basic_error_class, "%s", error.what());
  }
  return result;
}

static VALUE image_write_metadata(VALUE self) {
  Exiv2::Image* image;
  Data_Get_Struct(self, Exiv2::Image, image);

  try {
    image->writeMetadata();
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

static VALUE exif_data_add(VALUE self, VALUE key, VALUE value) {
  Exiv2::ExifData* data;
  Data_Get_Struct(self, Exiv2::ExifData, data);
  
  try {
    Exiv2::ExifKey exifKey = Exiv2::ExifKey(to_std_string(key));
    
    #if EXIV2_MAJOR_VERSION <= 0 && EXIV2_MINOR_VERSION <= 20
      Exiv2::TypeId typeId = Exiv2::ExifTags::tagType(exifKey.tag(), exifKey.ifdId());
    #else
      Exiv2::TypeId typeId = exifKey.defaultTypeId();
    #endif
    
    Exiv2::Value::AutoPtr v = Exiv2::Value::create(typeId);
    v->read(to_std_string(value));
    
    data->add(exifKey, v.get());
  }
  catch (Exiv2::BasicError<char> error) {
    rb_raise(basic_error_class, "%s", error.what());
  }
  return Qtrue;
}

static VALUE exif_data_delete(VALUE self, VALUE key) {
  Exiv2::ExifData* data;
  Data_Get_Struct(self, Exiv2::ExifData, data);
  
  Exiv2::ExifKey exifKey = Exiv2::ExifKey(to_std_string(key));
  Exiv2::ExifData::iterator pos = data->findKey(exifKey);
  if(pos == data->end()) return Qfalse;
  data->erase(pos);
  
  return Qtrue;
}


// Exiv2::IptcData methods

static VALUE iptc_data_each(VALUE self) {
  return metadata_each<Exiv2::IptcData>(self);
}

static VALUE iptc_data_add(VALUE self, VALUE key, VALUE value) {
  Exiv2::IptcData* data;
  Data_Get_Struct(self, Exiv2::IptcData, data);
  try {
    Exiv2::IptcKey iptcKey  = Exiv2::IptcKey(to_std_string(key));
    Exiv2::TypeId typeId    = Exiv2::IptcDataSets::dataSetType(iptcKey.tag(), iptcKey.record());
    
    Exiv2::Value::AutoPtr v = Exiv2::Value::create(typeId);
    v->read(to_std_string(value));
    
    if(data->add(iptcKey, v.get())) {
      return Qfalse;
    }
  }
  catch (Exiv2::BasicError<char> error) {
    rb_raise(basic_error_class, "%s", error.what());
  }
  return Qtrue;
}

static VALUE iptc_data_delete(VALUE self, VALUE key) {
  Exiv2::IptcData* data;
  Data_Get_Struct(self, Exiv2::IptcData, data);
  
  Exiv2::IptcKey iptcKey = Exiv2::IptcKey(to_std_string(key));
  Exiv2::IptcData::iterator pos = data->findKey(iptcKey);
  if(pos == data->end()) return Qfalse;
  data->erase(pos);
  
  return Qtrue;
}
 
// Exiv2::XmpData methods

static VALUE xmp_data_each(VALUE self) {
  return metadata_each<Exiv2::XmpData>(self);
}

static VALUE xmp_data_add(VALUE self, VALUE key, VALUE value) {
  Exiv2::XmpData* data;
  Data_Get_Struct(self, Exiv2::XmpData, data);
  try {
    Exiv2::XmpKey xmpKey = Exiv2::XmpKey(to_std_string(key));
    Exiv2::TypeId typeId = Exiv2::XmpProperties::propertyType(xmpKey);
    
    Exiv2::Value::AutoPtr v = Exiv2::Value::create(typeId);
    v->read(to_std_string(value));
    if(data->add(xmpKey, v.get())) {
      return Qfalse;
    }
  }
  catch (Exiv2::BasicError<char> error) {
    rb_raise(basic_error_class, "%s", error.what());
  }
  return Qtrue;
}

static VALUE xmp_data_delete(VALUE self, VALUE key) {
  Exiv2::XmpData* data;
  Data_Get_Struct(self, Exiv2::XmpData, data);
  
  Exiv2::XmpKey xmpKey = Exiv2::XmpKey(to_std_string(key));
  Exiv2::XmpData::iterator pos = data->findKey(xmpKey);
  if(pos == data->end()) return Qfalse;
  data->erase(pos);
  
  return Qtrue;
}

static VALUE xmp_register_ns(VALUE self, VALUE uri, VALUE ns) {
  try {
    Exiv2::XmpProperties::registerNs(to_std_string(uri), to_std_string(ns));
  }
  catch (Exiv2::BasicError<char> error) {
    rb_raise(basic_error_class, "%s", error.what());
  }
  return Qtrue;
}
