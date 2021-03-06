// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: msg.proto

#ifndef PROTOBUF_msg_2eproto__INCLUDED
#define PROTOBUF_msg_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3003001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
namespace pt {
class broadcast_register_service;
class broadcast_register_serviceDefaultTypeInternal;
extern broadcast_register_serviceDefaultTypeInternal _broadcast_register_service_default_instance_;
class obj_addr;
class obj_addrDefaultTypeInternal;
extern obj_addrDefaultTypeInternal _obj_addr_default_instance_;
class obj_service;
class obj_serviceDefaultTypeInternal;
extern obj_serviceDefaultTypeInternal _obj_service_default_instance_;
class req_register_service;
class req_register_serviceDefaultTypeInternal;
extern req_register_serviceDefaultTypeInternal _req_register_service_default_instance_;
class req_service;
class req_serviceDefaultTypeInternal;
extern req_serviceDefaultTypeInternal _req_service_default_instance_;
class rsp_register_service;
class rsp_register_serviceDefaultTypeInternal;
extern rsp_register_serviceDefaultTypeInternal _rsp_register_service_default_instance_;
class rsp_service;
class rsp_serviceDefaultTypeInternal;
extern rsp_serviceDefaultTypeInternal _rsp_service_default_instance_;
}  // namespace pt

namespace pt {

namespace protobuf_msg_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static void InitDefaultsImpl();
  static void Shutdown();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_msg_2eproto

// ===================================================================

class obj_addr : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pt.obj_addr) */ {
 public:
  obj_addr();
  virtual ~obj_addr();

  obj_addr(const obj_addr& from);

  inline obj_addr& operator=(const obj_addr& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const obj_addr& default_instance();

  static inline const obj_addr* internal_default_instance() {
    return reinterpret_cast<const obj_addr*>(
               &_obj_addr_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(obj_addr* other);

  // implements Message ----------------------------------------------

  inline obj_addr* New() const PROTOBUF_FINAL { return New(NULL); }

  obj_addr* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const obj_addr& from);
  void MergeFrom(const obj_addr& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(obj_addr* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string ip = 1;
  void clear_ip();
  static const int kIpFieldNumber = 1;
  const ::std::string& ip() const;
  void set_ip(const ::std::string& value);
  #if LANG_CXX11
  void set_ip(::std::string&& value);
  #endif
  void set_ip(const char* value);
  void set_ip(const char* value, size_t size);
  ::std::string* mutable_ip();
  ::std::string* release_ip();
  void set_allocated_ip(::std::string* ip);

  // int32 port = 2;
  void clear_port();
  static const int kPortFieldNumber = 2;
  ::google::protobuf::int32 port() const;
  void set_port(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:pt.obj_addr)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr ip_;
  ::google::protobuf::int32 port_;
  mutable int _cached_size_;
  friend struct protobuf_msg_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class obj_service : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pt.obj_service) */ {
 public:
  obj_service();
  virtual ~obj_service();

  obj_service(const obj_service& from);

  inline obj_service& operator=(const obj_service& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const obj_service& default_instance();

  static inline const obj_service* internal_default_instance() {
    return reinterpret_cast<const obj_service*>(
               &_obj_service_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(obj_service* other);

  // implements Message ----------------------------------------------

  inline obj_service* New() const PROTOBUF_FINAL { return New(NULL); }

  obj_service* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const obj_service& from);
  void MergeFrom(const obj_service& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(obj_service* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .pt.obj_addr addrs = 3;
  int addrs_size() const;
  void clear_addrs();
  static const int kAddrsFieldNumber = 3;
  const ::pt::obj_addr& addrs(int index) const;
  ::pt::obj_addr* mutable_addrs(int index);
  ::pt::obj_addr* add_addrs();
  ::google::protobuf::RepeatedPtrField< ::pt::obj_addr >*
      mutable_addrs();
  const ::google::protobuf::RepeatedPtrField< ::pt::obj_addr >&
      addrs() const;

  // string sname = 2;
  void clear_sname();
  static const int kSnameFieldNumber = 2;
  const ::std::string& sname() const;
  void set_sname(const ::std::string& value);
  #if LANG_CXX11
  void set_sname(::std::string&& value);
  #endif
  void set_sname(const char* value);
  void set_sname(const char* value, size_t size);
  ::std::string* mutable_sname();
  ::std::string* release_sname();
  void set_allocated_sname(::std::string* sname);

  // int32 sid = 1;
  void clear_sid();
  static const int kSidFieldNumber = 1;
  ::google::protobuf::int32 sid() const;
  void set_sid(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:pt.obj_service)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::pt::obj_addr > addrs_;
  ::google::protobuf::internal::ArenaStringPtr sname_;
  ::google::protobuf::int32 sid_;
  mutable int _cached_size_;
  friend struct protobuf_msg_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class req_service : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pt.req_service) */ {
 public:
  req_service();
  virtual ~req_service();

  req_service(const req_service& from);

  inline req_service& operator=(const req_service& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const req_service& default_instance();

  static inline const req_service* internal_default_instance() {
    return reinterpret_cast<const req_service*>(
               &_req_service_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    2;

  void Swap(req_service* other);

  // implements Message ----------------------------------------------

  inline req_service* New() const PROTOBUF_FINAL { return New(NULL); }

  req_service* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const req_service& from);
  void MergeFrom(const req_service& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(req_service* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // int32 sid = 1;
  void clear_sid();
  static const int kSidFieldNumber = 1;
  ::google::protobuf::int32 sid() const;
  void set_sid(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:pt.req_service)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::int32 sid_;
  mutable int _cached_size_;
  friend struct protobuf_msg_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class rsp_service : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pt.rsp_service) */ {
 public:
  rsp_service();
  virtual ~rsp_service();

  rsp_service(const rsp_service& from);

  inline rsp_service& operator=(const rsp_service& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const rsp_service& default_instance();

  static inline const rsp_service* internal_default_instance() {
    return reinterpret_cast<const rsp_service*>(
               &_rsp_service_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    3;

  void Swap(rsp_service* other);

  // implements Message ----------------------------------------------

  inline rsp_service* New() const PROTOBUF_FINAL { return New(NULL); }

  rsp_service* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const rsp_service& from);
  void MergeFrom(const rsp_service& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(rsp_service* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .pt.obj_service services = 1;
  int services_size() const;
  void clear_services();
  static const int kServicesFieldNumber = 1;
  const ::pt::obj_service& services(int index) const;
  ::pt::obj_service* mutable_services(int index);
  ::pt::obj_service* add_services();
  ::google::protobuf::RepeatedPtrField< ::pt::obj_service >*
      mutable_services();
  const ::google::protobuf::RepeatedPtrField< ::pt::obj_service >&
      services() const;

  // @@protoc_insertion_point(class_scope:pt.rsp_service)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::pt::obj_service > services_;
  mutable int _cached_size_;
  friend struct protobuf_msg_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class req_register_service : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pt.req_register_service) */ {
 public:
  req_register_service();
  virtual ~req_register_service();

  req_register_service(const req_register_service& from);

  inline req_register_service& operator=(const req_register_service& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const req_register_service& default_instance();

  static inline const req_register_service* internal_default_instance() {
    return reinterpret_cast<const req_register_service*>(
               &_req_register_service_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    4;

  void Swap(req_register_service* other);

  // implements Message ----------------------------------------------

  inline req_register_service* New() const PROTOBUF_FINAL { return New(NULL); }

  req_register_service* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const req_register_service& from);
  void MergeFrom(const req_register_service& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(req_register_service* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // int32 sid = 1;
  void clear_sid();
  static const int kSidFieldNumber = 1;
  ::google::protobuf::int32 sid() const;
  void set_sid(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:pt.req_register_service)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::int32 sid_;
  mutable int _cached_size_;
  friend struct protobuf_msg_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class rsp_register_service : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pt.rsp_register_service) */ {
 public:
  rsp_register_service();
  virtual ~rsp_register_service();

  rsp_register_service(const rsp_register_service& from);

  inline rsp_register_service& operator=(const rsp_register_service& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const rsp_register_service& default_instance();

  static inline const rsp_register_service* internal_default_instance() {
    return reinterpret_cast<const rsp_register_service*>(
               &_rsp_register_service_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    5;

  void Swap(rsp_register_service* other);

  // implements Message ----------------------------------------------

  inline rsp_register_service* New() const PROTOBUF_FINAL { return New(NULL); }

  rsp_register_service* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const rsp_register_service& from);
  void MergeFrom(const rsp_register_service& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(rsp_register_service* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // int32 sid = 1;
  void clear_sid();
  static const int kSidFieldNumber = 1;
  ::google::protobuf::int32 sid() const;
  void set_sid(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:pt.rsp_register_service)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::int32 sid_;
  mutable int _cached_size_;
  friend struct protobuf_msg_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class broadcast_register_service : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pt.broadcast_register_service) */ {
 public:
  broadcast_register_service();
  virtual ~broadcast_register_service();

  broadcast_register_service(const broadcast_register_service& from);

  inline broadcast_register_service& operator=(const broadcast_register_service& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const broadcast_register_service& default_instance();

  static inline const broadcast_register_service* internal_default_instance() {
    return reinterpret_cast<const broadcast_register_service*>(
               &_broadcast_register_service_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    6;

  void Swap(broadcast_register_service* other);

  // implements Message ----------------------------------------------

  inline broadcast_register_service* New() const PROTOBUF_FINAL { return New(NULL); }

  broadcast_register_service* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const broadcast_register_service& from);
  void MergeFrom(const broadcast_register_service& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(broadcast_register_service* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // .pt.obj_service service = 1;
  bool has_service() const;
  void clear_service();
  static const int kServiceFieldNumber = 1;
  const ::pt::obj_service& service() const;
  ::pt::obj_service* mutable_service();
  ::pt::obj_service* release_service();
  void set_allocated_service(::pt::obj_service* service);

  // @@protoc_insertion_point(class_scope:pt.broadcast_register_service)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::pt::obj_service* service_;
  mutable int _cached_size_;
  friend struct protobuf_msg_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// obj_addr

// string ip = 1;
inline void obj_addr::clear_ip() {
  ip_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& obj_addr::ip() const {
  // @@protoc_insertion_point(field_get:pt.obj_addr.ip)
  return ip_.GetNoArena();
}
inline void obj_addr::set_ip(const ::std::string& value) {
  
  ip_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pt.obj_addr.ip)
}
#if LANG_CXX11
inline void obj_addr::set_ip(::std::string&& value) {
  
  ip_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:pt.obj_addr.ip)
}
#endif
inline void obj_addr::set_ip(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  ip_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pt.obj_addr.ip)
}
inline void obj_addr::set_ip(const char* value, size_t size) {
  
  ip_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pt.obj_addr.ip)
}
inline ::std::string* obj_addr::mutable_ip() {
  
  // @@protoc_insertion_point(field_mutable:pt.obj_addr.ip)
  return ip_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* obj_addr::release_ip() {
  // @@protoc_insertion_point(field_release:pt.obj_addr.ip)
  
  return ip_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void obj_addr::set_allocated_ip(::std::string* ip) {
  if (ip != NULL) {
    
  } else {
    
  }
  ip_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ip);
  // @@protoc_insertion_point(field_set_allocated:pt.obj_addr.ip)
}

// int32 port = 2;
inline void obj_addr::clear_port() {
  port_ = 0;
}
inline ::google::protobuf::int32 obj_addr::port() const {
  // @@protoc_insertion_point(field_get:pt.obj_addr.port)
  return port_;
}
inline void obj_addr::set_port(::google::protobuf::int32 value) {
  
  port_ = value;
  // @@protoc_insertion_point(field_set:pt.obj_addr.port)
}

// -------------------------------------------------------------------

// obj_service

// int32 sid = 1;
inline void obj_service::clear_sid() {
  sid_ = 0;
}
inline ::google::protobuf::int32 obj_service::sid() const {
  // @@protoc_insertion_point(field_get:pt.obj_service.sid)
  return sid_;
}
inline void obj_service::set_sid(::google::protobuf::int32 value) {
  
  sid_ = value;
  // @@protoc_insertion_point(field_set:pt.obj_service.sid)
}

// string sname = 2;
inline void obj_service::clear_sname() {
  sname_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& obj_service::sname() const {
  // @@protoc_insertion_point(field_get:pt.obj_service.sname)
  return sname_.GetNoArena();
}
inline void obj_service::set_sname(const ::std::string& value) {
  
  sname_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pt.obj_service.sname)
}
#if LANG_CXX11
inline void obj_service::set_sname(::std::string&& value) {
  
  sname_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:pt.obj_service.sname)
}
#endif
inline void obj_service::set_sname(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  sname_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pt.obj_service.sname)
}
inline void obj_service::set_sname(const char* value, size_t size) {
  
  sname_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pt.obj_service.sname)
}
inline ::std::string* obj_service::mutable_sname() {
  
  // @@protoc_insertion_point(field_mutable:pt.obj_service.sname)
  return sname_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* obj_service::release_sname() {
  // @@protoc_insertion_point(field_release:pt.obj_service.sname)
  
  return sname_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void obj_service::set_allocated_sname(::std::string* sname) {
  if (sname != NULL) {
    
  } else {
    
  }
  sname_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), sname);
  // @@protoc_insertion_point(field_set_allocated:pt.obj_service.sname)
}

// repeated .pt.obj_addr addrs = 3;
inline int obj_service::addrs_size() const {
  return addrs_.size();
}
inline void obj_service::clear_addrs() {
  addrs_.Clear();
}
inline const ::pt::obj_addr& obj_service::addrs(int index) const {
  // @@protoc_insertion_point(field_get:pt.obj_service.addrs)
  return addrs_.Get(index);
}
inline ::pt::obj_addr* obj_service::mutable_addrs(int index) {
  // @@protoc_insertion_point(field_mutable:pt.obj_service.addrs)
  return addrs_.Mutable(index);
}
inline ::pt::obj_addr* obj_service::add_addrs() {
  // @@protoc_insertion_point(field_add:pt.obj_service.addrs)
  return addrs_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::pt::obj_addr >*
obj_service::mutable_addrs() {
  // @@protoc_insertion_point(field_mutable_list:pt.obj_service.addrs)
  return &addrs_;
}
inline const ::google::protobuf::RepeatedPtrField< ::pt::obj_addr >&
obj_service::addrs() const {
  // @@protoc_insertion_point(field_list:pt.obj_service.addrs)
  return addrs_;
}

// -------------------------------------------------------------------

// req_service

// int32 sid = 1;
inline void req_service::clear_sid() {
  sid_ = 0;
}
inline ::google::protobuf::int32 req_service::sid() const {
  // @@protoc_insertion_point(field_get:pt.req_service.sid)
  return sid_;
}
inline void req_service::set_sid(::google::protobuf::int32 value) {
  
  sid_ = value;
  // @@protoc_insertion_point(field_set:pt.req_service.sid)
}

// -------------------------------------------------------------------

// rsp_service

// repeated .pt.obj_service services = 1;
inline int rsp_service::services_size() const {
  return services_.size();
}
inline void rsp_service::clear_services() {
  services_.Clear();
}
inline const ::pt::obj_service& rsp_service::services(int index) const {
  // @@protoc_insertion_point(field_get:pt.rsp_service.services)
  return services_.Get(index);
}
inline ::pt::obj_service* rsp_service::mutable_services(int index) {
  // @@protoc_insertion_point(field_mutable:pt.rsp_service.services)
  return services_.Mutable(index);
}
inline ::pt::obj_service* rsp_service::add_services() {
  // @@protoc_insertion_point(field_add:pt.rsp_service.services)
  return services_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::pt::obj_service >*
rsp_service::mutable_services() {
  // @@protoc_insertion_point(field_mutable_list:pt.rsp_service.services)
  return &services_;
}
inline const ::google::protobuf::RepeatedPtrField< ::pt::obj_service >&
rsp_service::services() const {
  // @@protoc_insertion_point(field_list:pt.rsp_service.services)
  return services_;
}

// -------------------------------------------------------------------

// req_register_service

// int32 sid = 1;
inline void req_register_service::clear_sid() {
  sid_ = 0;
}
inline ::google::protobuf::int32 req_register_service::sid() const {
  // @@protoc_insertion_point(field_get:pt.req_register_service.sid)
  return sid_;
}
inline void req_register_service::set_sid(::google::protobuf::int32 value) {
  
  sid_ = value;
  // @@protoc_insertion_point(field_set:pt.req_register_service.sid)
}

// -------------------------------------------------------------------

// rsp_register_service

// int32 sid = 1;
inline void rsp_register_service::clear_sid() {
  sid_ = 0;
}
inline ::google::protobuf::int32 rsp_register_service::sid() const {
  // @@protoc_insertion_point(field_get:pt.rsp_register_service.sid)
  return sid_;
}
inline void rsp_register_service::set_sid(::google::protobuf::int32 value) {
  
  sid_ = value;
  // @@protoc_insertion_point(field_set:pt.rsp_register_service.sid)
}

// -------------------------------------------------------------------

// broadcast_register_service

// .pt.obj_service service = 1;
inline bool broadcast_register_service::has_service() const {
  return this != internal_default_instance() && service_ != NULL;
}
inline void broadcast_register_service::clear_service() {
  if (GetArenaNoVirtual() == NULL && service_ != NULL) delete service_;
  service_ = NULL;
}
inline const ::pt::obj_service& broadcast_register_service::service() const {
  // @@protoc_insertion_point(field_get:pt.broadcast_register_service.service)
  return service_ != NULL ? *service_
                         : *::pt::obj_service::internal_default_instance();
}
inline ::pt::obj_service* broadcast_register_service::mutable_service() {
  
  if (service_ == NULL) {
    service_ = new ::pt::obj_service;
  }
  // @@protoc_insertion_point(field_mutable:pt.broadcast_register_service.service)
  return service_;
}
inline ::pt::obj_service* broadcast_register_service::release_service() {
  // @@protoc_insertion_point(field_release:pt.broadcast_register_service.service)
  
  ::pt::obj_service* temp = service_;
  service_ = NULL;
  return temp;
}
inline void broadcast_register_service::set_allocated_service(::pt::obj_service* service) {
  delete service_;
  service_ = service;
  if (service) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:pt.broadcast_register_service.service)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


}  // namespace pt

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_msg_2eproto__INCLUDED
