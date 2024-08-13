// Generated by the protocol buffer compiler.  DO NOT EDIT!
// NO CHECKED-IN PROTOBUF GENCODE
// source: contact.proto
// Protobuf C++ Version: 5.27.2

#include "contact.pb.h"

#include <algorithm>
#include <type_traits>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/generated_message_tctable_impl.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;

inline constexpr Contact::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : a_user_id_{},
        b_user_id_{},
        _cached_size_{0},
        _oneof_case_{} {}

template <typename>
PROTOBUF_CONSTEXPR Contact::Contact(::_pbi::ConstantInitialized)
    : _impl_(::_pbi::ConstantInitialized()) {}
struct ContactDefaultTypeInternal {
  PROTOBUF_CONSTEXPR ContactDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~ContactDefaultTypeInternal() {}
  union {
    Contact _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 ContactDefaultTypeInternal _Contact_default_instance_;
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_contact_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_contact_2eproto = nullptr;
const ::uint32_t
    TableStruct_contact_2eproto::offsets[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
        protodesc_cold) = {
        ~0u,  // no _has_bits_
        PROTOBUF_FIELD_OFFSET(::Contact, _internal_metadata_),
        ~0u,  // no _extensions_
        PROTOBUF_FIELD_OFFSET(::Contact, _impl_._oneof_case_[0]),
        ~0u,  // no _weak_field_map_
        ~0u,  // no _inlined_string_donated_
        ~0u,  // no _split_
        ~0u,  // no sizeof(Split)
        ::_pbi::kInvalidFieldOffsetTag,
        ::_pbi::kInvalidFieldOffsetTag,
        ::_pbi::kInvalidFieldOffsetTag,
        ::_pbi::kInvalidFieldOffsetTag,
        PROTOBUF_FIELD_OFFSET(::Contact, _impl_.a_user_id_),
        PROTOBUF_FIELD_OFFSET(::Contact, _impl_.b_user_id_),
};

static const ::_pbi::MigrationSchema
    schemas[] ABSL_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
        {0, -1, -1, sizeof(::Contact)},
};
static const ::_pb::Message* const file_default_instances[] = {
    &::_Contact_default_instance_._instance,
};
const char descriptor_table_protodef_contact_2eproto[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
    protodesc_cold) = {
    "\n\rcontact.proto\"\215\001\n\007Contact\022\027\n\ra_user_id"
    "_int\030\001 \001(\005H\000\022\032\n\020a_user_id_string\030\002 \001(\tH\000"
    "\022\027\n\rb_user_id_int\030\003 \001(\005H\001\022\032\n\020b_user_id_s"
    "tring\030\004 \001(\tH\001B\013\n\ta_user_idB\013\n\tb_user_idb"
    "\006proto3"
};
static ::absl::once_flag descriptor_table_contact_2eproto_once;
PROTOBUF_CONSTINIT const ::_pbi::DescriptorTable descriptor_table_contact_2eproto = {
    false,
    false,
    167,
    descriptor_table_protodef_contact_2eproto,
    "contact.proto",
    &descriptor_table_contact_2eproto_once,
    nullptr,
    0,
    1,
    schemas,
    file_default_instances,
    TableStruct_contact_2eproto::offsets,
    file_level_enum_descriptors_contact_2eproto,
    file_level_service_descriptors_contact_2eproto,
};
// ===================================================================

class Contact::_Internal {
 public:
  static constexpr ::int32_t kOneofCaseOffset =
      PROTOBUF_FIELD_OFFSET(::Contact, _impl_._oneof_case_);
};

Contact::Contact(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:Contact)
}
inline PROTOBUF_NDEBUG_INLINE Contact::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility, ::google::protobuf::Arena* arena,
    const Impl_& from, const ::Contact& from_msg)
      : a_user_id_{},
        b_user_id_{},
        _cached_size_{0},
        _oneof_case_{from._oneof_case_[0], from._oneof_case_[1]} {}

Contact::Contact(
    ::google::protobuf::Arena* arena,
    const Contact& from)
    : ::google::protobuf::Message(arena) {
  Contact* const _this = this;
  (void)_this;
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  new (&_impl_) Impl_(internal_visibility(), arena, from._impl_, from);
  switch (a_user_id_case()) {
    case A_USER_ID_NOT_SET:
      break;
      case kAUserIdInt:
        _impl_.a_user_id_.a_user_id_int_ = from._impl_.a_user_id_.a_user_id_int_;
        break;
      case kAUserIdString:
        new (&_impl_.a_user_id_.a_user_id_string_) decltype(_impl_.a_user_id_.a_user_id_string_){arena, from._impl_.a_user_id_.a_user_id_string_};
        break;
  }
  switch (b_user_id_case()) {
    case B_USER_ID_NOT_SET:
      break;
      case kBUserIdInt:
        _impl_.b_user_id_.b_user_id_int_ = from._impl_.b_user_id_.b_user_id_int_;
        break;
      case kBUserIdString:
        new (&_impl_.b_user_id_.b_user_id_string_) decltype(_impl_.b_user_id_.b_user_id_string_){arena, from._impl_.b_user_id_.b_user_id_string_};
        break;
  }

  // @@protoc_insertion_point(copy_constructor:Contact)
}
inline PROTOBUF_NDEBUG_INLINE Contact::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : a_user_id_{},
        b_user_id_{},
        _cached_size_{0},
        _oneof_case_{} {}

inline void Contact::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
}
Contact::~Contact() {
  // @@protoc_insertion_point(destructor:Contact)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void Contact::SharedDtor() {
  ABSL_DCHECK(GetArena() == nullptr);
  if (has_a_user_id()) {
    clear_a_user_id();
  }
  if (has_b_user_id()) {
    clear_b_user_id();
  }
  _impl_.~Impl_();
}

void Contact::clear_a_user_id() {
// @@protoc_insertion_point(one_of_clear_start:Contact)
  ::google::protobuf::internal::TSanWrite(&_impl_);
  switch (a_user_id_case()) {
    case kAUserIdInt: {
      // No need to clear
      break;
    }
    case kAUserIdString: {
      _impl_.a_user_id_.a_user_id_string_.Destroy();
      break;
    }
    case A_USER_ID_NOT_SET: {
      break;
    }
  }
  _impl_._oneof_case_[0] = A_USER_ID_NOT_SET;
}

void Contact::clear_b_user_id() {
// @@protoc_insertion_point(one_of_clear_start:Contact)
  ::google::protobuf::internal::TSanWrite(&_impl_);
  switch (b_user_id_case()) {
    case kBUserIdInt: {
      // No need to clear
      break;
    }
    case kBUserIdString: {
      _impl_.b_user_id_.b_user_id_string_.Destroy();
      break;
    }
    case B_USER_ID_NOT_SET: {
      break;
    }
  }
  _impl_._oneof_case_[1] = B_USER_ID_NOT_SET;
}


const ::google::protobuf::MessageLite::ClassData*
Contact::GetClassData() const {
  PROTOBUF_CONSTINIT static const ::google::protobuf::MessageLite::
      ClassDataFull _data_ = {
          {
              &_table_.header,
              nullptr,  // OnDemandRegisterArenaDtor
              nullptr,  // IsInitialized
              PROTOBUF_FIELD_OFFSET(Contact, _impl_._cached_size_),
              false,
          },
          &Contact::MergeImpl,
          &Contact::kDescriptorMethods,
          &descriptor_table_contact_2eproto,
          nullptr,  // tracker
      };
  ::google::protobuf::internal::PrefetchToLocalCache(&_data_);
  ::google::protobuf::internal::PrefetchToLocalCache(_data_.tc_table);
  return _data_.base();
}
PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<0, 4, 0, 48, 2> Contact::_table_ = {
  {
    0,  // no _has_bits_
    0, // no _extensions_
    4, 0,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967280,  // skipmap
    offsetof(decltype(_table_), field_entries),
    4,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    &_Contact_default_instance_._instance,
    nullptr,  // post_loop_handler
    ::_pbi::TcParser::GenericFallback,  // fallback
    #ifdef PROTOBUF_PREFETCH_PARSE_TABLE
    ::_pbi::TcParser::GetTable<::Contact>(),  // to_prefetch
    #endif  // PROTOBUF_PREFETCH_PARSE_TABLE
  }, {{
    {::_pbi::TcParser::MiniParse, {}},
  }}, {{
    65535, 65535
  }}, {{
    // int32 a_user_id_int = 1;
    {PROTOBUF_FIELD_OFFSET(Contact, _impl_.a_user_id_.a_user_id_int_), _Internal::kOneofCaseOffset + 0, 0,
    (0 | ::_fl::kFcOneof | ::_fl::kInt32)},
    // string a_user_id_string = 2;
    {PROTOBUF_FIELD_OFFSET(Contact, _impl_.a_user_id_.a_user_id_string_), _Internal::kOneofCaseOffset + 0, 0,
    (0 | ::_fl::kFcOneof | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // int32 b_user_id_int = 3;
    {PROTOBUF_FIELD_OFFSET(Contact, _impl_.b_user_id_.b_user_id_int_), _Internal::kOneofCaseOffset + 4, 0,
    (0 | ::_fl::kFcOneof | ::_fl::kInt32)},
    // string b_user_id_string = 4;
    {PROTOBUF_FIELD_OFFSET(Contact, _impl_.b_user_id_.b_user_id_string_), _Internal::kOneofCaseOffset + 4, 0,
    (0 | ::_fl::kFcOneof | ::_fl::kUtf8String | ::_fl::kRepAString)},
  }},
  // no aux_entries
  {{
    "\7\0\20\0\20\0\0\0"
    "Contact"
    "a_user_id_string"
    "b_user_id_string"
  }},
};

PROTOBUF_NOINLINE void Contact::Clear() {
// @@protoc_insertion_point(message_clear_start:Contact)
  ::google::protobuf::internal::TSanWrite(&_impl_);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  clear_a_user_id();
  clear_b_user_id();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

::uint8_t* Contact::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Contact)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  switch (a_user_id_case()) {
    case kAUserIdInt: {
      target = ::google::protobuf::internal::WireFormatLite::
          WriteInt32ToArrayWithField<1>(
              stream, this->_internal_a_user_id_int(), target);
      break;
    }
    case kAUserIdString: {
      const std::string& _s = this->_internal_a_user_id_string();
      ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
          _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Contact.a_user_id_string");
      target = stream->WriteStringMaybeAliased(2, _s, target);
      break;
    }
    default:
      break;
  }
  switch (b_user_id_case()) {
    case kBUserIdInt: {
      target = ::google::protobuf::internal::WireFormatLite::
          WriteInt32ToArrayWithField<3>(
              stream, this->_internal_b_user_id_int(), target);
      break;
    }
    case kBUserIdString: {
      const std::string& _s = this->_internal_b_user_id_string();
      ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
          _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Contact.b_user_id_string");
      target = stream->WriteStringMaybeAliased(4, _s, target);
      break;
    }
    default:
      break;
  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Contact)
  return target;
}

::size_t Contact::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Contact)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  switch (a_user_id_case()) {
    // int32 a_user_id_int = 1;
    case kAUserIdInt: {
      total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(
          this->_internal_a_user_id_int());
      break;
    }
    // string a_user_id_string = 2;
    case kAUserIdString: {
      total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                      this->_internal_a_user_id_string());
      break;
    }
    case A_USER_ID_NOT_SET: {
      break;
    }
  }
  switch (b_user_id_case()) {
    // int32 b_user_id_int = 3;
    case kBUserIdInt: {
      total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(
          this->_internal_b_user_id_int());
      break;
    }
    // string b_user_id_string = 4;
    case kBUserIdString: {
      total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                      this->_internal_b_user_id_string());
      break;
    }
    case B_USER_ID_NOT_SET: {
      break;
    }
  }
  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}


void Contact::MergeImpl(::google::protobuf::MessageLite& to_msg, const ::google::protobuf::MessageLite& from_msg) {
  auto* const _this = static_cast<Contact*>(&to_msg);
  auto& from = static_cast<const Contact&>(from_msg);
  ::google::protobuf::Arena* arena = _this->GetArena();
  // @@protoc_insertion_point(class_specific_merge_from_start:Contact)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (const uint32_t oneof_from_case = from._impl_._oneof_case_[0]) {
    const uint32_t oneof_to_case = _this->_impl_._oneof_case_[0];
    const bool oneof_needs_init = oneof_to_case != oneof_from_case;
    if (oneof_needs_init) {
      if (oneof_to_case != 0) {
        _this->clear_a_user_id();
      }
      _this->_impl_._oneof_case_[0] = oneof_from_case;
    }

    switch (oneof_from_case) {
      case kAUserIdInt: {
        _this->_impl_.a_user_id_.a_user_id_int_ = from._impl_.a_user_id_.a_user_id_int_;
        break;
      }
      case kAUserIdString: {
        if (oneof_needs_init) {
          _this->_impl_.a_user_id_.a_user_id_string_.InitDefault();
        }
        _this->_impl_.a_user_id_.a_user_id_string_.Set(from._internal_a_user_id_string(), arena);
        break;
      }
      case A_USER_ID_NOT_SET:
        break;
    }
  }
  if (const uint32_t oneof_from_case = from._impl_._oneof_case_[1]) {
    const uint32_t oneof_to_case = _this->_impl_._oneof_case_[1];
    const bool oneof_needs_init = oneof_to_case != oneof_from_case;
    if (oneof_needs_init) {
      if (oneof_to_case != 0) {
        _this->clear_b_user_id();
      }
      _this->_impl_._oneof_case_[1] = oneof_from_case;
    }

    switch (oneof_from_case) {
      case kBUserIdInt: {
        _this->_impl_.b_user_id_.b_user_id_int_ = from._impl_.b_user_id_.b_user_id_int_;
        break;
      }
      case kBUserIdString: {
        if (oneof_needs_init) {
          _this->_impl_.b_user_id_.b_user_id_string_.InitDefault();
        }
        _this->_impl_.b_user_id_.b_user_id_string_.Set(from._internal_b_user_id_string(), arena);
        break;
      }
      case B_USER_ID_NOT_SET:
        break;
    }
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void Contact::CopyFrom(const Contact& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Contact)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}


void Contact::InternalSwap(Contact* PROTOBUF_RESTRICT other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_.a_user_id_, other->_impl_.a_user_id_);
  swap(_impl_.b_user_id_, other->_impl_.b_user_id_);
  swap(_impl_._oneof_case_[0], other->_impl_._oneof_case_[0]);
  swap(_impl_._oneof_case_[1], other->_impl_._oneof_case_[1]);
}

::google::protobuf::Metadata Contact::GetMetadata() const {
  return ::google::protobuf::Message::GetMetadataImpl(GetClassData()->full());
}
// @@protoc_insertion_point(namespace_scope)
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::std::false_type
    _static_init2_ PROTOBUF_UNUSED =
        (::_pbi::AddDescriptors(&descriptor_table_contact_2eproto),
         ::std::false_type{});
#include "google/protobuf/port_undef.inc"
